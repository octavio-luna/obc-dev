// Palermo  University - Engineering School
// LABSAT OBC main.c
 
// FreeRTOS includes
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
// ISISpace hardware abstraction layer includes
#include <hal/Timing/WatchDogTimer.h>
#include <hal/Drivers/LED.h>
#include <hal/boolean.h>
#include <hal/Utility/util.h>
#include <hal/version/version.h>
// cpu includes
#include <at91/utility/trace.h>
#include <at91/peripherals/cp15/cp15.h>
#include <at91/utility/exithandler.h>
#include <at91/commons.h>
// project includes
#include "ObcGlobals.h"
#include "PowerManager.h"
#include "TimerManager.h"
#include "LogManager.h"
#include "CSPManager.h"
#include "SDManager.h"
#include "DevelTest.h"
// Misc includes
#include <stdlib.h>
// forward declarations
void InitTask(void*);

//////////////////////////////////////////////////////////////////////////////
int main(void) {

	// Enable the Instruction cache of the ARM9 core. Keep the MMU and Data Cache disabled.
	CP15_Enable_I_Cache();

	#ifdef  FLIGHT_VERSION
		UPLOG_INFO("LABSAT main: start");
	#else
		TRACE_CONFIGURE_ISP(DBGU_STANDARD, 2000000, BOARD_MCK);
		UPLOG_INFO("LABSAT main: start -- Compiled on %s %s -- HAL v%s.%s.%s", __DATE__, __TIME__,
					HalVersionMajor,HalVersionMinor, HalVersionRevision);
		LED_start();
		LED_wave(1);
		LED_waveReverse(1);
		LED_wave(1);
		LED_waveReverse(1);
	#endif

	// Next task initializes everything and then finishes (short lived task).
	// Will only run after scheduler is started.
	xTaskCreate(InitTask,"InitTask",basic_STACK_DEPTH,NULL,basic_TASK_PRIORITY,NULL);
	UPLOG_INFO("main: scheduler go");
	// will never return from next function
	vTaskStartScheduler();
	// This function should never get here, nevertheless, please make sure that this last call doesn't get optimized away
	exit(0);
}

//////////////////////////////////////////////////////////////////////////////

// Next task initiates all managers after task scheduler is up.
// (some managers initializers need the task scheduler to be running so
//  they need to be run from a scheduled task themselves)
void InitTask(void* args) {
	UPLOG_INFO("InitTask starting");
	// OBCWatchdog
	// The actual obc watchdog has already started, this only initializes the watchdog-kick interface.
	WDT_start();

	// need to init this to be able to log to a file
	SDManagerInit();

	LogManagerInit();

	// Needs the scheduler to be already started
	TimerManagerInit(0);

	PowerManagerInit();

	CSPManagerInit(CSP_UART_BUS);

	// TaskManagerInit();

	#ifndef  FLIGHT_VERSION
	DevelTestInit();
	#endif
	UPLOG_INFO("InitTask end");
	f_releaseFS();
	// Unlike most other tasks, this task ends. So we release the task resources.
	vTaskDelete(NULL);
}
