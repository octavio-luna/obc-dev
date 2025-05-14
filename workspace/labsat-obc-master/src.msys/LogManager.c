/* hal and project includes */
#include <hcc/api_fat.h>
#include <at91/peripherals/dbgu/dbgu.h>
#include <hal/Timing/Time.h>
#include <hal/Timing/RTT.h>
#include <hal/Timing/RTC.h>
#include "LogManager.h"
/* FreeRTOS includes */
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h> 
/* DO NOT FORGET hal/at91/src/utility/stdio.c IN THE SOURCE FILES !! */
/* (to prevent code size bloat of standard stdio lib) */
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

typedef unsigned int ui32;
typedef struct { int n; char txt[MAXLOGLINE]; } logQueueItem;

char runLogLevel    					 = RUN_LOGLEVEL;
static ui32 maxLogFileSize			 = SDLOG_MAXFILESIZE;
static ui32 logRotateNum			 = SDLOG_ROTATENUM;
static char			 logNonBlocking = LOG_NONBLOCKING;
static const char* sdPath         = SDLOG_PATH;
static xTaskHandle  logTaskHdl = 0;
static xQueueHandle logQHandle = 0;
static F_FILE*				 logFH = 0;

//////////////////////////////////////////////////////////////////////////////
// macro definitions

// Some options can only be enabled when on flight version, and some others only on earth lab version
#ifdef FLIGHT_VERSION
	#define __DBGU_WRITE_LOG__(q)
#else /* earth lab version */
	inline static void __DBGU_WRITE_LOG__(const char* p) { while( *p ) DBGU_PutChar(*p++); }
#endif
// some options are only available if logging to SD is enabled
#ifdef SDLOG
	inline static void __SD_WRITE_LOG__(const char* p, int n) { if( logFH ) f_write(p,n,1,logFH); }
#else
	#define __SD_WRITE_LOG__(p,n)
#endif

///////////////////////////////////////////////////////////////////////////
// private functions

inline static void logCombined(const char* t, int n) {
	__SD_WRITE_LOG__(t,n);
	__DBGU_WRITE_LOG__(t);
}

int logHdr(char* buf) {
	Time t;
	int n = 0;
	ui32 h = (ui32)xTaskGetCurrentTaskHandle();
	if( Time_get(&t)!=0 ) {
		unsigned int s  = RTT_GetStatus();
		unsigned int tt = RTT_GetTime();
		n = sprintf(buf,"%u(%u) [%u] ",tt,s,h);
	} else {
		n = sprintf(buf,"%u-%u-%u %u:%u:%u [%u] ",
				2000+(ui32)(t.year+2000),(ui32)t.month,(ui32)t.date,
				(ui32)t.hours,(ui32)t.minutes,(ui32)t.seconds, h);
	}
	return n;
}


void UPLOG(const char* str,...) {
	va_list args;
	va_start(args, str);
	logQueueItem* li = pvPortMalloc(sizeof(logQueueItem));
	li->n = logHdr(li->txt);
	li->n += vsnprintf(li->txt+li->n,MAXLOGLINE-2-li->n,str,args); 
	li->txt[li->n++] = '\n'; li->txt[li->n] = 0;
	if( logNonBlocking )  {
		xQueueSend(logQHandle,li,portMAX_DELAY);
	} else {
		// Why use heap mem for txt instead of stack mem when blocking?
		// Because in freertos tasks we may have little stack mem. 
		logCombined(li->txt,li->n);
		vPortFree(li);
	}
}


void LogManagerTask(void* q) {
	static const char* lmtxt = __FUNCTION__, *starting = "starting\n", *ending = "ending\n";
	logQueueItem* li;	
	xQueueHandle tq = logQHandle;
	int n = 0;
	if( f_enterFS()!=F_NO_ERROR ) { __DBGU_WRITE_LOG__(lmtxt); __DBGU_WRITE_LOG__(" enter FS error\n"); goto endOfLogTask; }
	logCombined(lmtxt,strlen(lmtxt)); logCombined(starting,strlen(starting)); // need strlen instead of sizeof here
	for(;;) {
		if( pdPASS!=xQueueReceive(tq, &li, LOGQUEUE_WAIT_TICKS) ) {
			#ifdef SDLOG
			// timeout, check size
			if( n>=maxLogFileSize ) { if( logRotateCheck() ) n = 0; }
			#endif
			continue;
		}
		if( li==0 ) break; // signaled to end task
		logCombined(li->txt,li->n);
		n += li->n;
		#ifdef SDLOG
			if( n>=maxLogFileSize ) { if( logRotateCheck() ) n = 0; }
		#endif
		vPortFree(li);
	}
	endOfLogTask:
	// flush queue and delete it
	while( pdPASS==xQueueReceive(tq,&li,0) ) vPortFree(li);
	vQueueDelete(tq);
	// bye message
	logCombined(ending,sizeof(ending)); logCombined(ending,sizeof(ending));
	f_releaseFS();
	// Release the task resources
	vTaskDelete(NULL);
}

//////////////////////////////////////////////////////////////////////////////
// Public functions


int LogManagerInit() {
	static const char* ownStr = __FUNCTION__;
	int ret = 0;
	#ifdef SDLOG
		if( f_enterFS()!=F_NO_ERROR ) {
			ret = -1;
			__DBGU_WRITE_LOG__(ownStr); __DBGU_WRITE_LOG__("error registering task to hcc");
		} else {
			logFH = f_open(sdPath,"a");
			if( logFH==0 ) {
				ret = -2;
				__DBGU_WRITE_LOG__(ownStr); __DBGU_WRITE_LOG__("error opening logfile");
			} 
		}
	#endif
	if( logNonBlocking ) {
   	logQHandle = xQueueCreate(LOG_MAXQUEUE,sizeof(void*)); // queue of pointers to logItems
   	if( pdPASS!=xTaskCreate(LogManagerTask,"LogManagerTask",LOG_STACK_SIZE,NULL,LOG_PRIORITY,&logTaskHdl) ) {
			logNonBlocking = 0;
			if( logQHandle ) { vQueueDelete( logQHandle ); logQHandle = 0; }
			logTaskHdl = 0;
			ret = -2;
			UPLOG_ALERT("%serror creating task => using blocking logs",ownStr);
		}
	} else {
		// in this case, old queue is deleted by old log task
		logQHandle = 0;
		logTaskHdl = 0;
	}
	// Do not unregister from filesystem management. InitTask will continue logging.
	// #ifdef SDLOG
	//  	f_releaseFS();
	// #endif
	return ret;
}

int LogManagerReinit(const char* _sdPath,char _nonBlocking,
							unsigned int _maxLogFileSize,unsigned int _logRotateNum) {
	if( logNonBlocking && logTaskHdl ) {
		void *li = 0;
		if( xQueueSend(logQHandle,&li,portMAX_DELAY)!=pdTRUE ) { // signal logTask to finish and delete queue
			// Could not signal task to stop! Stop it by force
			vTaskDelete(logTaskHdl);
		}
		logQHandle = 0;
		logTaskHdl = 0;
	}
	#ifdef SDLOG
	if( logFH ) { f_close(logFH); logFH = 0; }
	#endif

	logRotateNum   = _logRotateNum;
	maxLogFileSize = _maxLogFileSize;
	logNonBlocking = _nonBlocking;
	sdPath         = _sdPath;

	return LogManagerInit();
}


void logRotate() {
	if( !logFH ) return;
	f_close(logFH); logFH = 0;
	char n1[F_MAXPATHNAME];
	int k = strlen(sdPath);
	if( k>F_MAXPATHNAME-17 ) k = F_MAXPATHNAME-17;
	strncpy(n1,sdPath,k); n1[k] = 0;
	Time t;
	if( Time_get(&t)!=0 ) {
		unsigned int s  = RTT_GetStatus();
		unsigned int tt = RTT_GetTime();
		sprintf(n1+k,"-%u-%u",tt,s);
	} else {
		sprintf(n1+k,"-%u%02u%02u-%02u%02u%02u",
				2000+(ui32)(t.year+2000),(ui32)t.month,(ui32)t.date,
				(ui32)t.hours,(ui32)t.minutes,(ui32)t.seconds);
	}
	f_rename(sdPath,n1);
	logFH = f_open(sdPath,"a");
	if( logFH==0 ) __DBGU_WRITE_LOG__("LogRotate() could not open new logfile");
	unsigned int count;
	do {
		F_FIND f;
		n1[k] = '*'; n1[k+1] = 0;
		int n = f_findfirst(n1,&f);
		unsigned short cdate = 65535, ctime = 65535;
		count = 0;
		while( n==F_NO_ERROR ) {
			++count;
			if( ( f.cdate<cdate ) || ( f.cdate==cdate && f.ctime<ctime ) ) {
				cdate = f.cdate; ctime = f.ctime;
				strncpy(n1,f.filename,F_MAXPATHNAME); n1[F_MAXPATHNAME-1] = 0;
			}
			n = f_findnext(&f);
		}
		if( count>logRotateNum ) {
			f_delete(n1);
			--count;
		}
	} while( count>logRotateNum );
}


char logRotateCheck() {
	if( !logFH ) return 0;
	if( maxLogFileSize==0 || logRotateNum==0 ) return 0;
	F_STAT s; s.filesize = 0;
	f_fstat(logFH,&s);
	if( s.filesize<maxLogFileSize ) return 0;
	logRotate();
	return 1;
}


void setLogRunLevel(char c) { runLogLevel = c; }

char *freertosSt[] = { "running","ready","blocked","suspended","deleted","invalid",0 };

/* Function vTaskGetInfoi(...) is not available in this freertos distribution.
void UPLOG_TaskStatus(xTaskHandle taskHandle) {
	TaskStatus_t taskDetails;
	vTaskGetInfo(taskHandle,&taskDetails,pdTRUE,eInvalid);
	// si habilitamos configGENERATE_RUN_TIME_STATS en FreeRTOS.h podemos tambien obtener taskDetails.ulRunTimeCounter 
	UPLOG("taskStatus: name='%s' num=%u state=%s prioCurr=%d minStackDepth=%u\n",
				taskDetails.pcTaskName,
				taskDetails.xTaskNumber,
				freertosSt[taskDetails.eCurrentState],
				taskDetails.uxCurrentPriority,
				taskDetails.usStackHighWaterMark);
}
*/


#if (configUSE_TRACE_FACILITY==1)
int UPLOG_TasksStatus() {
	const char* ownStr = __FUNCTION__;
	unsigned long totalRunTime = 0;
	unsigned portBASE_TYPE i, taskCount = uxTaskGetNumberOfTasks();
	xTaskStatusType *taskStats = (xTaskStatusType*) pvPortMalloc( taskCount*sizeof(xTaskStatusType) );
	if( taskStats==0 ) { UPLOG_CRIT("%s out of mem",ownStr); return -1; }
	taskCount = uxTaskGetSystemState( taskStats, taskCount, &totalRunTime ); // get info about each task
	if( taskCount==0 ) { UPLOG_WARNING("%s taskCount increased during call",ownStr); vPortFree(taskStats); return 0; }
#if (configGENERATE_RUN_TIME_STATS==1)
	totalRunTime /= 100UL;
	char *p = totalRunTime ? "%" : "";
	for( i=0; i<taskCount; ++i ) {
		unsigned int runPercent = taskStats[i].ulRunTimeCounter;
		if( totalRunTime ) runPercent /=totalRunTime;
		UPLOG("taskState: name='%s' num=%u state=%s prioCurr=%d minStackDepth=%u runTime%s=%d",
				taskStats[i].pcTaskName,
				taskStats[i].xTaskNumber,
				freertosSt[taskStats[i].eCurrentState],
				taskStats[i].uxCurrentPriority,
				taskStats[i].usStackHighWaterMark,
				p,runPercent );
	}
#else
	for( i=0; i<taskCount; ++i ) {
		UPLOG("taskState: name='%s' num=%u state=%s prioCurr=%d minStackDepth=%u",
				taskStats[i].pcTaskName,
				taskStats[i].xTaskNumber,
				freertosSt[taskStats[i].eCurrentState],
				taskStats[i].uxCurrentPriority,
				taskStats[i].usStackHighWaterMark);
	}
#endif
	vPortFree( taskStats );
	return taskCount;
}
#else
	#define UPLOG_TasksStatus()
#endif

