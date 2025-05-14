#ifndef LABSAT_TIMERMANAGER_H
#define LABSAT_TIMERMANAGER_H

#include <freertos/FreeRTOS.h>
#include <hal/Timing/Time.h>
#include "ObcGlobals.h"

#define TM_SYNC_INTERVAL 60  // seconds between RTT and RTC synchronization
#define TM_WAKEUP_INTERVAL 1  // seconds between checks for expired timers
#define TM_STACK_SIZE (basic_STACK_DEPTH * 4) // basic is 4096, check if enough
#define TM_PRIORITY (configMAX_PRIORITIES-1)  // maximum configurable

#define FIRST_TIMER_EXEC_NOW 1
#define INFINITE_REPEAT 1073741824

// Initialize the LabsatTimer manager.
// Returns: 0 on success, 1 if RTT/RTC is not working, 2 if setTime fails,
//          3 if FreeRTOS scheduler is not already running, 4 if attempted to initialize twice.
//				5 if labsatTimer manager task failed
// Param t is a Time structure pointer to the desired time to set the RTC. If null, RTC is kept as is.
char TimerManagerInit(const Time* t);

// Schedule a new labsat timer.
// Param 'when' is a unix timestamp.
// If needed can be generated using Time_convertTimeToEpoch(const Time* t) from hal/Timing/Time.h
// If param when<currentTime or when==FIRST_TIMER_EXEC_NOW, then it will be executed on the next TimerManager wakeup.
// If param when==0 then it will be executed an interval from currentTime.
// If param interval==0 the timer will not repeat. 
// if param interval==INFINITE_REPEAT, then the timer will repeat for ever.
// The callback function will receive the actual execution time (which may differ from the requested time,
// and the privDataPointer. This function is in charge of freeing privData memory if needed.
// If the callback return value differs from 0, it can reconfigure its repetition settings:
//   if negative it will prevent any further repetitions.
//   if positive it will rewrite the interval value and add 1 to the pending repetitions count. 
void TimerManagerAdd(unsigned int when, int (*callback)(unsigned int _when,void* _privData), unsigned int interval,
							 unsigned int repetitionCount, void* privData, const char* name );

// get the number of pending timers (updated once every wakeup)
unsigned int TimerManagerCount();

#endif
