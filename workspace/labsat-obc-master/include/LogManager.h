#ifndef LOGMANAGER_H
#define LOGMANAGER_H

#include "ObcGlobals.h"

// We define a COMPILE_LOGLEVEL, and a RUN_LOGLEVEL.
// Log calls with levels higher than COMPILE_LOGLEVEL will disappear from the compiled code.
// Log calls with levels higher than RUN_LOGLEVEL will not be executed but still be part of log.
// Anyhow the will use zero processor clocks, not even for the calls made for calculating their
//  parameters before the actual call.
// Levels are: EMERG 0, ALERT 1, CRIT 2, ERR 3, WARNING 4, NOTICE 5, INFO 6, DEBUG 7

// In general we want flight software to run even if finding severe errors during
// initialization or regular operations. So errors should never hung the software.
// Although we may occasionally restart a manager, a task, a module or the
// complete software.

#define COMPILE_LOGLEVEL 7
#define RUN_LOGLEVEL 7
extern char runLogLevel;
typedef void* xTaskHandle; // same as in freertos/task.h

// Maximum log line length
#define MAXLOGLINE 192

// Queue logs and return at once? or wait till written to file?
// (if log queue is full UPLOG calls will still block until there is space in queue)
#define LOG_NONBLOCKING 1
// Nonblocking logs require the following definitions
// LOG Task definitions (only for non-blocking logs)
#define LOG_STACK_SIZE basic_STACK_DEPTH	// check if 4096 is enough
#define LOG_PRIORITY (configMAX_PRIORITIES-3)
#define LOGQUEUE_WAIT_TICKS (10*configTICK_RATE_HZ) // ticks
#define LOG_MAXQUEUE 32


// Enable logging to file in SD card
//#undef SDLOG
#define SDLOG 1
// SD card log needs the following definitions
#define SDLOG_PATH "A:/var/log/syslog" /* may have many /var dirs on diff partitions*/
#define SDLOG_MAXFILESIZE 4192 /* will later change this to a larger size */
#define SDLOG_ROTATENUM   3

//////////////////////////////////////////////////////////////////////////////
// Public functions

// Initialize log for the first time after booting with the default values
int LogManagerInit();

void setLogRunLevel(char c);

// ReInitialize log. Several log parameters can be changed from the default values.
// If _sdPath=0 then no logging will be done to SDcard or ramdisk.
// If _maxLogFileSize is nonzero, logs files will be rotated. And _logRotateNum
// is the max number of rotated files to maintain.
// If _nonBlocking is nonzero, then logs will be non blocking and conducted through
// a freeRTOS queue. If queue is full, logs will be blocking anyhow.
int LogManagerReinit(const char* _sdPath,char _nonBlocking,
							unsigned int _maxLogFileSize,unsigned int _logRotateNum);

// Force rotation of current logFile without checking for size
void logRotate();
// Rotate if curret file larger than maxLogFileSize (returns 0 if rotates)
char logRotateCheck();

// Do not call this directly, use one of the macros below
void UPLOG(const char* str,...);

// Log Task status. If taskHandler is 0 it logs current task status.
int UPLOG_TasksStatus();


//////////////////////////////////////////////////////////////////////////////
// Log macros and definitions

#define LOG_EMERG 0
#define LOG_ALERT 1
#define LOG_CRIT 2
#define LOG_ERR 3
#define LOG_WARNING 4
#define LOG_NOTICE 5
#define LOG_INFO 6
#define LOG_DEBUG 7

// Next macro is aways defined regardless of COMPILE_LOGLEVEL
#define UPLOG_EMERG(...) UPLOG(__VA_ARGS__) // this one is aways compiled regardless of COMPILE_LOGLEVEL
// Next macros are only compiled when COMPILE_LOGLEVEL is higher than a threshold
#if COMPILE_LOGLEVEL>=LOG_ALERT
	#define UPLOG_ALERT(...)   if( runLogLevel>=1 ) UPLOG(__VA_ARGS__)
#else
	#define UPLOG_ALERT(...)
#endif
#if COMPILE_LOGLEVEL>=LOG_CRIT
	#define UPLOG_CRIT(...)    if( runLogLevel>=2 ) UPLOG(__VA_ARGS__)
#else
	#define UPLOG_CRIT(...)
#endif
#if COMPILE_LOGLEVEL>=LOG_ERR
	#define UPLOG_ERR(...)     if( runLogLevel>=3 ) UPLOG(__VA_ARGS__)
#else
	#define UPLOG_ERR(...)
#endif
#if COMPILE_LOGLEVEL>=LOG_WARNING
	#define UPLOG_WARNING(...) if( runLogLevel>=4 ) UPLOG(__VA_ARGS__)
#else
	#define UPLOG_WARNING(...)
#endif
#if COMPILE_LOGLEVEL>=LOG_NOTICE
	#define UPLOG_NOTICE(...)  if( runLogLevel>=5 ) UPLOG(__VA_ARGS__)
#else
	#define UPLOG_NOTICE(...)
#endif
#if COMPILE_LOGLEVEL>=LOG_INFO
	#define UPLOG_INFO(...)    if( runLogLevel>=6 ) UPLOG(__VA_ARGS__)
#else
	#define UPLOG_INFO(...)
#endif
#if COMPILE_LOGLEVEL>=LOG_DEBUG
	#define UPLOG_DEBUG(...)   if( runLogLevel>=7 ) UPLOG(__VA_ARGS__)
	#define UPDEBUG(...)       UPLOG_DEBUG(__VA_ARGS__)
	#define X						UPLOG_DEBUG("debug mark %s %s",__FUNCTION__,__LINE__);
#else
	#define UPLOG_DEBUG(...)
	#define UPDEBUG(...)
	#define X
#endif



#endif
