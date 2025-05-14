#include "TimerManager.h"
#include <freertos/semphr.h>
#include <freertos/task.h>
#include <string.h>

typedef struct LabsatTimer_ {
	// Labsat timers are indexed by the next field: a unix timestamp, useful only until 19-jan-2038
	unsigned int when;
	// When the time comes, the following callback will be executed by the Labsat timer main task.
	// Do not execute blocking or long proceses on this callback. If need be to execute a blocking or long work,
	// signal an execution to another freertos task.
	// It is not executed inside mutex/sempahore locked sections, and not executed inside an ISR.
	// the stack size for the task executing the callback is TM_STACK_SIZE
	int (*callback)(unsigned int _when, void* priv);
	// how much time (seconds) between repetitions
	unsigned int interval;
	// how many times it should be repeated;
	unsigned int count;
	// private data
	void* privData;
	// pointer to other timers in ordered list
	struct LabsatTimer_ *prev,*next;
	// tiemr identifier
	char id[8];
} LabsatTimer;


// data Private to this module:
static LabsatTimer* ltFirst = 0;
static LabsatTimer* toAdd = 0;
static unsigned int ltTimerCount = 0;
static xSemaphoreHandle ltMutex = 0;
static unsigned int currentTime = 0; // unix timestamp
static unsigned int tickInterval = 0; // wakeup time expressed in freertos ticks
static xTaskHandle ltTaskHandle;


void insertTimerPrivate(LabsatTimer* lt) {
	lt->prev = lt->next = 0;
	if( ltFirst==0 ) {
		ltFirst = lt;
		return;
	}
	if( lt->when < ltFirst->when ) {
		lt->next = ltFirst;
		ltFirst->prev = lt;
		ltFirst = lt;
		return;
	} 
	LabsatTimer* aux;
	for(aux=ltFirst ; ; aux=aux->next ) {
		if( lt->when < aux->when ) {
			lt->prev = aux->prev;
			lt->next = aux;
			aux->prev->next = lt;
			aux->prev = lt;
			break;
		}
		// when >= aux->when
		if( aux->next==0 ) {
			aux->next = lt;
			lt->prev = aux;
			break;
		}
	}
}


// LabsatTimer task
void TimerManagerTask(void* params) {
	LabsatTimer *lt,*aux;
	portTickType ltLastWakeTime = xTaskGetTickCount();
	for(;;) {
		// critical section, keep as short as possible
		xSemaphoreTake( ltMutex, (portTickType)portMAX_DELAY );
			aux = toAdd;
			toAdd = 0;
		xSemaphoreGive(ltMutex);
		while( aux ) {
			lt = aux;
			aux = lt->next;
			insertTimerPrivate(lt);
		}
		// catchup loop
		// cannot accept insertions during the catchup loop
		Time_getUnixEpoch( &currentTime );
   	while( ltFirst && ltFirst->when <= currentTime ) {
			lt = ltFirst;
			ltFirst = lt->next;
			int ret = lt->callback(currentTime,lt->privData);
			if( ret<0 ) lt->count = 0; // forcing to stop repetitions
			else {
				if( ret>0 ) lt->interval = (unsigned int)ret; 
				if( lt->count!=INFINITE_REPEAT && ret==0 ) --(lt->count);
			}
			if( lt->count ) {
				lt->when += lt->interval;
				insertTimerPrivate(lt);
			} else {
				vPortFree(lt); // callback is in charge of deleting lt->privData if needed
				xSemaphoreGive(ltMutex);
					--ltTimerCount;
				xSemaphoreGive(ltMutex);
			}
		}
		vTaskDelayUntil( &ltLastWakeTime, tickInterval );
	}
	vTaskDelete(NULL);
}



// Initialize the LabsatTimer manager.
// Returns: 0 on success, 1 if RTT/RTC is not working, 2 if setTime fails,
//          3 if FreeRTOS scheduler is not already running, 4 if attempted to initialize twice.
//				5 if labsatTimer manager task failed
// Param t is a Time structure pointer to the desired time to set the RTC. If null, RTC is kept as is.
// Param wakeUpInterval is the time interval between wakeups in the TimerManager task (catch-up wakeup interval).
// Param syncInterval is the time interval between synchronising RTC and RTT in seconds.
char TimerManagerInit(const Time* t) {
	if( currentTime ) return 4; // already initialized, fail
	tickInterval = TM_WAKEUP_INTERVAL*configTICK_RATE_HZ;
   ltMutex = xSemaphoreCreateMutex();
	char ret = Time_start(t,TM_SYNC_INTERVAL);
	if( ret ) return ret; // hal RTC/RTT initialization failed
	Time_getUnixEpoch( &currentTime );
	if( pdPASS!=xTaskCreate(TimerManagerTask,"TimerManagerTask",TM_STACK_SIZE,NULL,TM_PRIORITY,&ltTaskHandle) ) return 5;
	return 0;
}


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
void TimerManagerAdd(unsigned int when, int (*callback)(unsigned int_when, void* _privData), unsigned int interval,
							 unsigned int repetitionCount, void* privData, const char* name)
{
	// Thread save allocation. assume heap_4 or heap_5.
	// Note that because of its being thread safe, we prefer allocating outside the mutex.
	LabsatTimer* lt = pvPortMalloc( sizeof(LabsatTimer) );
   if( when ) lt->when = when; else lt->when = currentTime + interval;
	lt->callback	= callback;
	lt->interval	= interval;
	lt->count		= repetitionCount;
	lt->privData	= privData;
	if( name ) { strncpy(lt->id,name,7); lt->id[7] = 0; } else lt->id[0] = 0;
	// Safe critical section. Keep it as short as possible
	xSemaphoreTake( ltMutex, (portTickType)portMAX_DELAY );
		lt->next = toAdd;
		toAdd = lt;
		++ltTimerCount;
	xSemaphoreGive(ltMutex);
}


unsigned int TimerManagerCount() { return ltTimerCount; }
