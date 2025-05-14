// The only public commands for power manager that can be accessed from the outside of the manager are:
//
// char PowerManagerInit() for initializing the manager
//
// char PowerManagerAddRequest(commandCode,when,callback,cdata) to schedule a power management command,
//   (can also be scheduled for now). Command codes are defined below.
//   All commands are actually executed from the power manager own task. If need be to access the command
//   results, these should be accessed from the callback.

#ifndef POWERMANAGER_H
#define POWERMANAGER_H

// the following file includes recursively satellite-subsystems/isismepsv2_ivid7_piu_types.h
// and satellite-subsystems/common_types.h
#include <satellite-subsystems/isismepsv2_ivid7_piu.h>
#include "ObcGlobals.h"

#define PM_STACK_SIZE basic_STACK_DEPTH	// check if 4096 is enough
#define PM_PRIORITY (configMAX_PRIORITIES-1)  // this is the max configurable priority
#define PM_WDG_TIMEOUT 300 //secs
#define PM_TTC_TIMEOUT (PM_WDG_TIMEOUT*65/100) //secs
#define PM_QUEUE_WAIT_TICKS ( PM_WDG_TIMEOUT/8*configTICK_RATE_HZ) // ticks


// type definitions (using isismepsv2_ivid7_piu.h, isismepsv2_ivid7_piu_types.h common_types.h)
typedef union __attribute__((__packed__)) {
	char raw[10];
	isismepsv2_ivid7_piu__outputbusgroupoff__to_t			obusOff;
	isismepsv2_ivid7_piu__outputbusgroupon__to_t 			obusOn; 
	isismepsv2_ivid7_piu__outputbusgroupstate__to_t			obusState;
	isismepsv2_ivid7_piu__setconfigurationparameter__to_t	setConfParam;
	isismepsv2_ivid7_piu__imeps_channel_t						channel_idx;
} commandReqData;  // 10 bytes

// Do not allocate this directly. Will be allocated by satellite-subsystems library without requesting unused mem.
// It is defined just to have a wildcard pointer to response data
typedef union __attribute__((__packed__)) {
	char raw[274];
isismepsv2_ivid7_piu__getconfigurationparameter__from_t				getConfParam;
isismepsv2_ivid7_piu__gethousekeepingeng__from_t						getHKe;
isismepsv2_ivid7_piu__gethousekeepingengincdb__from_t					getHKincdb;
isismepsv2_ivid7_piu__gethousekeepingengrunningavgincdb__from_t	getHKrunningavgincdb;
isismepsv2_ivid7_piu__gethousekeepingraw__from_t						getHKr;
isismepsv2_ivid7_piu__gethousekeepingrawincdb__from_t					getHKrinccdb;
isismepsv2_ivid7_piu__gethousekeepingrunningavg__from_t				getHKravg;
isismepsv2_ivid7_piu__getovercurrentfaultstate__from_t				getOvercurrentFaultSt;
isismepsv2_ivid7_piu__getsystemstatus__from_t							getSysStatus;
isismepsv2_ivid7_piu__resetconfigurationparameter__from_t			resetConfigParam;
isismepsv2_ivid7_piu__setconfigurationparameter__from_t				setConfParam;
isismepsv2_ivid7_piu__replyheader_t											replyHdrOut;
} commandRespData; // 274 bytes


// PowerManager will call this function after finishing a command.
// PowerManager is in charge of freeing response memory after executing callback.
// Param when will hold the current time of the response if the command was scheduled. Else it will be 0.
typedef void (*PowerManagerCmdCallback)(driver_error_t cmderr,unsigned int when,commandRespData* response);


typedef struct __attribute__((__packed__)) _PowerManagerRequest {
	unsigned int when; // 4 bytes
	PowerManagerCmdCallback callback; // sizeof(functionPointer): 4 bytes in 32bits arch, 8 bytes in 64 bits arch.
	commandReqData cdata; // 10 bytes
	unsigned char commandCode; // 1 byte. See below command code definitions
} PowerManagerRequest; // 19 bytes

// Command codes
#define PM_CC_NOP 0x02
#define PM_CC_CANCEL 0x04
#define PM_CC_RESETWATCHDOG 0x06
#define PM_CC_OUTPUTBUSGROUPON 0x10
#define PM_CC_OUTPUTBUSGROUPOFF 0x12
#define PM_CC_OUTPUTBUSGROUPSTATE 0x14
#define PM_CC_OUTPUTBUSCHANNELON 0x16
#define PM_CC_OUTPUTBUSCHANNELOFF 0x18
#define PM_CC_SWITCHTONOMINAL 0x30
#define PM_CC_SWITCHTOSAFETY 0x32
#define PM_CC_GETSYSTEMSTATUS 0x40
#define PM_CC_GETOVERCURRENTFAULTSTATE 0x42
#define PM_CC_GETCONFIGURATIONPARAMETER 0x82
#define PM_CC_SETCONFIGURATIONPARAMETER 0x84
#define PM_CC_RESETCONFIGURATIONPARAMETER 0x86
#define PM_CC_RESETCONFIGURATION 0x90
#define PM_CC_LOADCONFIGURATION 0x92
#define PM_CC_SAVECONFIGURATION 0x94
#define PM_CC_GETHOUSEKEEPINGRAW 0xA0
#define PM_CC_GETHOUSEKEEPINGRAWINCDB 0xA0
#define PM_CC_GETHOUSEKEEPINGENG 0xA2
#define PM_CC_GETHOUSEKEEPINGENGINCDB 0xA2
#define PM_CC_GETHOUSEKEEPINGENGRUNNINGAVGINCDB 0xA4
#define PM_CC_GETHOUSEKEEPINGRUNNINGAVG 0xA4
#define PM_CC_RESET 0xAA
#define PM_CC_CORRECTTIME 0xC4
#define PM_CC_ZERORESETCAUSECOUNTERS 0xC6
#define PM_CC_CANCEL 0x04
#define PM_CC_RESETWATCHDOG 0x06
#define PM_CC_OUTPUTBUSGROUPON 0x10
#define PM_CC_OUTPUTBUSGROUPOFF 0x12
#define PM_CC_OUTPUTBUSGROUPSTATE 0x14
#define PM_CC_OUTPUTBUSCHANNELON 0x16
#define PM_CC_OUTPUTBUSCHANNELOFF 0x18
#define PM_CC_SWITCHTONOMINAL 0x30
#define PM_CC_SWITCHTOSAFETY 0x32
#define PM_CC_GETSYSTEMSTATUS 0x40
#define PM_CC_GETOVERCURRENTFAULTSTATE 0x42
#define PM_CC_GETCONFIGURATIONPARAMETER 0x82
#define PM_CC_SETCONFIGURATIONPARAMETER 0x84
#define PM_CC_RESETCONFIGURATIONPARAMETER 0x86
#define PM_CC_RESETCONFIGURATION 0x90
#define PM_CC_LOADCONFIGURATION 0x92
#define PM_CC_SAVECONFIGURATION 0x94
#define PM_CC_GETHOUSEKEEPINGRAW 0xA0
#define PM_CC_GETHOUSEKEEPINGRAWINCDB 0xA0
#define PM_CC_GETHOUSEKEEPINGENG 0xA2
#define PM_CC_GETHOUSEKEEPINGENGINCDB 0xA2
#define PM_CC_GETHOUSEKEEPINGENGRUNNINGAVGINCDB 0xA4
#define PM_CC_GETHOUSEKEEPINGRUNNINGAVG 0xA4
#define PM_CC_RESET 0xAA
#define PM_CC_CORRECTTIME 0xC4
#define PM_CC_SYNCTIME    0xC4 // same as correcttime. We always sync EPS time to OBC RTC.
#define PM_CC_ZERORESETCAUSECOUNTERS 0xC6


// Public functions in this module:
char PowerManagerInit();

// Send command to power manager / EPS
// When response from the command is available, it will execute the callback
// (will be xecuted from power manager own task)
char PowerManagerAddRequest(unsigned int commandCode, unsigned int when,PowerManagerCmdCallback callback, commandReqData* cdata);

// Print resonse header, or system status.
// If strPtr==0 then it prints to regular log
// If strPtr!=0 and *strPtr!=0 it prints to the memory pointed by *strPtr
// If strPtr!=0 and *strPtr==0 it allocates memory and points *strPtr to it, and then prints there 
// resp is a pointer to the structure passed to the power manager callback.
// It returns the length of the generated string
int PowerManagerPrintRespHdr(char** strPtr,commandRespData* resp);
int PowerManagerPrintSysStatus(char** strPtr,commandRespData* resp);

#endif /* TASK_H_ */

