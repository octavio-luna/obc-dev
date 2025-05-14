#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <string.h>
#include "PowerManager.h"
#include "TimerManager.h"
#include "LogManager.h"

static xQueueHandle pmQHandle;
static xTaskHandle pmTaskHandle;
static unsigned int lastEpsCmdTstamp = 0;
uint8_t piu_index = 0;
const char* epsModeStr[] = { "startup","nominal","safety","emlopo",0 };
const char* epsResetCauseStr[] = { "power-on","watchdog","commanded","crlSysReset","emlopo",0 };
const char* epsRespStat[] = { "ok","rejected","invalidCmd","paramMissing","paramInvalid","invalidId","internalErr","","","","new",0 };
#define min(A,B) ( ( (A)<(B) ) ? (A) : (B) )

// these are defined in PowerManagerUart.c
driver_error_t uart_piu__getsystemstatus(isismepsv2_ivid7_piu__getsystemstatus__from_t* response);
driver_error_t uart_piu__resetwatchdog(isismepsv2_ivid7_piu__replyheader_t* response);
driver_error_t uart_piu__correcttime(int diff,isismepsv2_ivid7_piu__replyheader_t* reply);
driver_error_t uart_piu__outputbuschannelon(isismepsv2_ivid7_piu__imeps_channel_t channel_idx,isismepsv2_ivid7_piu__replyheader_t* response);
driver_error_t uart_piu__outputbuschanneloff(isismepsv2_ivid7_piu__imeps_channel_t channel_idx,isismepsv2_ivid7_piu__replyheader_t* response);
driver_error_t uart_piu__outputbusgroupon(isismepsv2_ivid7_piu__outputbusgroupon__to_t* obusOn,isismepsv2_ivid7_piu__replyheader_t* response);
driver_error_t uart_piu__outputbusgroupoff(isismepsv2_ivid7_piu__outputbusgroupoff__to_t* obusOff,isismepsv2_ivid7_piu__replyheader_t* response);
driver_error_t uart_piu__outputbusgroupstate(isismepsv2_ivid7_piu__outputbusgroupstate__to_t* obusState,isismepsv2_ivid7_piu__replyheader_t* response);
driver_error_t uart_piu__setconfigurationparameter(isismepsv2_ivid7_piu__setconfigurationparameter__to_t* setConfParam,isismepsv2_ivid7_piu__setconfigurationparameter__from_t* response);


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS (not to be called from outside power manager)
// (we declare them static functions as they will not need to be accessed from other files)


static unsigned int updateLastCommandTime() {
	Time_getUnixEpoch( &lastEpsCmdTstamp );
	return lastEpsCmdTstamp;
}

static void PowerManagerGetStatus(PowerManagerRequest* req) {
	isismepsv2_ivid7_piu__getsystemstatus__from_t response;
	updateLastCommandTime();
	driver_error_t error = isismepsv2_ivid7_piu__getsystemstatus(piu_index,&response);
	// see include/satellite-subsystems/common_types.h for driver error types
	if( error!=driver_error_none ) error=uart_piu__getsystemstatus(&response);
	if( req->callback ) req->callback(error,0,(commandRespData*)&response);
	// response se libera aqui
}

static void PowerManagerResetWatchdog(PowerManagerRequest* req) {
	isismepsv2_ivid7_piu__replyheader_t response;
	updateLastCommandTime();
   driver_error_t error = isismepsv2_ivid7_piu__resetwatchdog(piu_index,&response);
	if( error!=driver_error_none ) error=uart_piu__resetwatchdog(&response);
	if( req->callback ) req->callback(error,0,(commandRespData*)&response);
}


static int PowerManagerSyncEpsClock() {
	unsigned int t; int diff; driver_error_t error;
	isismepsv2_ivid7_piu__getsystemstatus__from_t response;
	isismepsv2_ivid7_piu__replyheader_t reply;
	error = isismepsv2_ivid7_piu__getsystemstatus(piu_index,&response);
	if( error!=driver_error_none ) error=uart_piu__getsystemstatus(&response);
	if( error!=driver_error_none ) return error;
	t = updateLastCommandTime();
	diff = t-response.fields.unix_time;
	if( 0==diff ) return 0;
	error = isismepsv2_ivid7_piu__correcttime(piu_index,diff,&reply);
	if( error!=driver_error_none ) error=uart_piu__correcttime(diff,&reply);
	if( error!=driver_error_none ) return error;
	if( 0!=reply.fields.cmderr ) return reply.fields.cmderr;
	return 0;
}

static void PowerManagerOutputBusChannelOn(PowerManagerRequest* req) {
	isismepsv2_ivid7_piu__replyheader_t response;
	updateLastCommandTime();
	driver_error_t error = isismepsv2_ivid7_piu__outputbuschannelon(piu_index,req->cdata.channel_idx,&response);
	if( error!=driver_error_none ) error = uart_piu__outputbuschannelon(req->cdata.channel_idx,&response);
	if( req->callback ) req->callback(error,0,(commandRespData*)&response);
	// response se libera aqui
}

static void PowerManagerOutputBusChannelOff(PowerManagerRequest* req) {
	isismepsv2_ivid7_piu__replyheader_t response;
	updateLastCommandTime();
	driver_error_t error = isismepsv2_ivid7_piu__outputbuschanneloff(piu_index,req->cdata.channel_idx,&response);
	if( error!=driver_error_none ) error = uart_piu__outputbuschanneloff(req->cdata.channel_idx,&response);
	if( req->callback ) req->callback(error,0,(commandRespData*)&response);
	// response se libera aqui
}

static void PowerManagerOutputBusGroupOn(PowerManagerRequest* req) {
	isismepsv2_ivid7_piu__replyheader_t response;
	updateLastCommandTime();
	driver_error_t error = isismepsv2_ivid7_piu__outputbusgroupon(piu_index,&(req->cdata.obusOn),&response);
	if( error!=driver_error_none ) error = uart_piu__outputbusgroupon(&(req->cdata.obusOn),&response);
	if( req->callback ) req->callback(error,0,(commandRespData*)&response);
	// response se libera aqui
}

static void PowerManagerOutputBusGroupOff(PowerManagerRequest* req) {
	isismepsv2_ivid7_piu__replyheader_t response;
	updateLastCommandTime();
	driver_error_t error = isismepsv2_ivid7_piu__outputbusgroupoff(piu_index,&(req->cdata.obusOff),&response);
	if( error!=driver_error_none ) error = uart_piu__outputbusgroupoff(&(req->cdata.obusOff),&response);
	if( req->callback ) req->callback(error,0,(commandRespData*)&response);
	// response se libera aqui
}

static void PowerManagerOutputBusGroupState(PowerManagerRequest* req) {
	isismepsv2_ivid7_piu__replyheader_t response;
	updateLastCommandTime();
	driver_error_t error = isismepsv2_ivid7_piu__outputbusgroupstate(piu_index,&(req->cdata.obusState),&response);
	if( error!=driver_error_none ) error = uart_piu__outputbusgroupstate(&(req->cdata.obusState),&response);
	if( req->callback ) req->callback(error,0,(commandRespData*)&response);
	// response se libera aqui
}

static void PowerManagerSetConfigParam(PowerManagerRequest* req) {
	isismepsv2_ivid7_piu__setconfigurationparameter__from_t response;
	updateLastCommandTime();
	driver_error_t error = isismepsv2_ivid7_piu__setconfigurationparameter(piu_index,&(req->cdata.setConfParam),&response);
	if( error!=driver_error_none ) error = uart_piu__setconfigurationparameter(&(req->cdata.setConfParam),&response);
	if( req->callback ) req->callback(error,0,(commandRespData*)&response);
	// response se libera aqui
}

static void PowerManagerExec(PowerManagerRequest* req) {
	switch(req->commandCode) {
		case PM_CC_GETSYSTEMSTATUS:
			PowerManagerGetStatus(req);
			break;
		case PM_CC_RESETWATCHDOG:
			PowerManagerResetWatchdog(req);
			break;
		case PM_CC_SYNCTIME:
			PowerManagerSyncEpsClock();
			break;
		case PM_CC_OUTPUTBUSGROUPON:
			PowerManagerOutputBusGroupOn(req);
			break;
		case PM_CC_OUTPUTBUSGROUPOFF:
			PowerManagerOutputBusGroupOff(req);
			break;
		case PM_CC_OUTPUTBUSCHANNELON:
			PowerManagerOutputBusChannelOn(req);
			break;
		case PM_CC_OUTPUTBUSCHANNELOFF:
			PowerManagerOutputBusChannelOff(req);
			break;
		case PM_CC_OUTPUTBUSGROUPSTATE:
			PowerManagerOutputBusGroupState(req);
			break;
		case PM_CC_SETCONFIGURATIONPARAMETER:
			PowerManagerSetConfigParam(req);
			break;
		default:
			break;
	}
}


int PowerManagerTimerCallback(unsigned int _when, void* privData) {
	// exec command now
	PowerManagerExec((PowerManagerRequest*)privData);
	vPortFree(privData);
	return 0;
}


static void PowerManagerTask(void* args) {
	PowerManagerRequest *req;
	unsigned int now;
	for(;;) {
		// check incoming commands queue
		if( pdTRUE != xQueueReceive(pmQHandle,&req,PM_QUEUE_WAIT_TICKS) ) {
			// Timeout. Kick EPS if needed
			Time_getUnixEpoch( &now );
			if( now-lastEpsCmdTstamp > (PM_WDG_TIMEOUT/4) ) PowerManagerResetWatchdog(0);
		} else if( req->when!=0 ) {
			// schedule command execution in the future.
			TimerManagerAdd(req->when,PowerManagerTimerCallback,0,1,(void*)req,"PowerManagerCallback");
			Time_getUnixEpoch( &now );
			if( now-lastEpsCmdTstamp > (PM_WDG_TIMEOUT/4) ) PowerManagerResetWatchdog(0);
		} else {
			// exec command now
			PowerManagerExec(req);
			vPortFree(req);
		}
	}
	vTaskDelete(NULL);
}

inline uint8_t  c2c(uint8_t c)  { return (uint8_t)(c+'0'); }
inline uint32_t c2u(uint8_t c)  { return (uint32_t)c; }
inline uint32_t s2u(uint16_t s) { return (uint32_t)s; }


/////////////////////////////////////////////////////////////////////////////
// Public functions

char PowerManagerInit() {
   pmQHandle = xQueueCreate(24,sizeof(PowerManagerRequest*));
   if( ! pmQHandle ) { UPLOG_ALERT("PowerManagerInit queue"); return 4; }

   if( pdPASS!=xTaskCreate(PowerManagerTask,"PowerManagerTask",PM_STACK_SIZE,NULL,PM_PRIORITY,&pmTaskHandle) )
   { UPLOG_ALERT("PowerManagerInit task"); return 5; }
	// schedule initial EPS clock synchronization with the OBC RTC.
	PowerManagerAddRequest(PM_CC_SYNCTIME,0,NULL,0);
	return 0;
}


char PowerManagerAddRequest(unsigned int commandCode, unsigned int when, PowerManagerCmdCallback callback, commandReqData* cdata) {
	PowerManagerRequest *req = pvPortMalloc(sizeof(PowerManagerRequest)); // ojo habia un error aca de Luis con el tamaño
	req->when = when;
	req->commandCode = commandCode;
	req->callback = callback;
	if( cdata ) memcpy(&(req->cdata),cdata,sizeof(commandReqData));
	else 			memset(&(req->cdata),0,sizeof(commandReqData));
	if( pdTRUE!=xQueueSendToBack(pmQHandle,req,portMAX_DELAY) ) {
		vPortFree(req);
		return -1;
	}
	return 0;
}

// Print resonse header
// If strPtr==0 then it prints to regular log
// If strPtr!=0 and *strPtr!=0 it prints to the memory pointed by *strPtr
// If strPtr!=0 and *strPtr==0 it allocates memory and points *strPtr to it, and then prints there 
// resp is a pointer to the structure passed to the power manager callback.
// It returns the length of the generated string
int PowerManagerPrintRespHdr(char** strPtr,commandRespData* resp) {
	if( resp==0 ) return -1;
	static const char sysSt[] = "EPS Resp stID=%x ivID=%x rc=%x boardID=%u stat=%s";
	unsigned int n = sizeof(sysSt)+48;
	char* str =  strPtr!=0 ? *strPtr : 0;
	if( str==0 ) str = (char*)pvPortMalloc(n);
	if( str==0 ) return -2;
	isismepsv2_ivid7_piu__replyheader_t *r= &(resp->getSysStatus.fields.reply_header);
	unsigned int respStat = min(r->fields.cmderr,(sizeof(epsRespStat)-1));
	n = snprintf(str,n,sysSt,c2u(r->fields.stid),c2u(r->fields.ivid),c2u(r->fields.rc),
						 c2u(r->fields.bid),epsRespStat[respStat] );
	if( !strPtr ) { UPLOG_INFO(str); vPortFree(str); }
	return n;
}

// Print system status
// If strPtr==0 then it prints to regular log
// If strPtr!=0 and *strPtr!=0 it prints to the memory pointed by *strPtr
// If strPtr!=0 and *strPtr==0 it allocates memory and points *strPtr to it, and then prints there 
// resp is a pointer to the structure passed to the power manager callback.
// It returns the length of the generated string
int PowerManagerPrintSysStatus(char** strPtr,commandRespData* resp) {
	if( resp==0 ) return -1;
	static const char sysSt[] = "EPS sysStatus mode=%s confChg=%c resetCause=%s uptimeSecs=%u err=%u pwrOnCnt=%u wdgRstCnt=%u cmdRstCnt=%u ctrlRstCnt=%u emlopoRstCnt=%u cmdElapsed=%u unixTime=%s %u-%u-%u %u:%u:%u";
	unsigned int n = sizeof(sysSt)+11+1+6+10+5*7+10+14+16;
	char* str =  strPtr!=0 ? *strPtr : 0;
	if( str==0 ) str = (char*)pvPortMalloc(n);
	if( str==0 ) return -2;
	isismepsv2_ivid7_piu__getsystemstatus__from_t *s = &(resp->getSysStatus);
	unsigned int respMode = min(s->fields.mode,(sizeof(epsModeStr)-1));
	unsigned int respResC = min(s->fields.reset_cause,(sizeof(epsResetCauseStr)-1));
	n = snprintf(str,n,sysSt,
		epsModeStr[respMode],
		c2c(s->fields.conf),
		epsResetCauseStr[respResC],
		s->fields.uptime,
		s2u(s->fields.error),
		s2u(s->fields.rc_cnt_pwron),
		s2u(s->fields.rc_cnt_wdg),
		s2u(s->fields.rc_cnt_cmd),
		s2u(s->fields.rc_cnt_pweron_mcu),
		s2u(s->fields.rc_cnt_emlopo),
		s2u(s->fields.prevcmd_elapsed),
		s->fields.unix_time,
		c2u(s->fields.unix_year)+2000,
		c2u(s->fields.unix_month),
		c2u(s->fields.unix_day),
		c2u(s->fields.unix_hour),
		c2u(s->fields.unix_minute),
		c2u(s->fields.unix_second)
	);
	if( !strPtr ) { UPLOG_INFO(str); vPortFree(str); }
	return n;
}
