#include <at91/utility/exithandler.h>
#include <at91/commons.h>
#include <at91/utility/trace.h>
#include <at91/peripherals/cp15/cp15.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>

#include <hal/boolean.h>
#include <hal/Drivers/LED.h>
#include <hal/Drivers/SPI.h>
#include <hal/Storage/FRAM.h>
#include <hal/Timing/Time.h>
#include <hal/Timing/WatchDogTimer.h>
#include <hal/Utility/util.h>
#include <hal/version/version.h>

#include <hcc/api_fat.h>
#include <hcc/api_fs_err.h>
#include <hcc/api_hcc_mem.h>
#include <hcc/api_mdriver_atmel_mcipdc.h>

#include <mission-support/version/version.h>

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "Demos/LogDemo.h"
#include "Demos/ParamsDBdemo.h"
#include "Demos/PersistentListDemo.h"

#define ENABLE_MAIN_TRACES 1
#if ENABLE_MAIN_TRACES
	#define MAIN_TRACE_INFO			TRACE_INFO
	#define MAIN_TRACE_DEBUG		TRACE_DEBUG
	#define MAIN_TRACE_WARNING		TRACE_WARNING
	#define MAIN_TRACE_ERROR		TRACE_ERROR
	#define MAIN_TRACE_FATAL		TRACE_FATAL
#else
	#define MAIN_TRACE_INFO(...)	{ }
	#define MAIN_TRACE_DEBUG(...)	{ }
	#define MAIN_TRACE_WARNING(...)	{ }
	#define MAIN_TRACE_ERROR		TRACE_ERROR
	#define MAIN_TRACE_FATAL		TRACE_FATAL
#endif

Boolean selectAndExecuteTest()
{
	int selection = 0;
	Boolean offerMoreTests = TRUE;

	printf("\n\rSelect a demo to run: \n\r");
	printf(" 1) ParamsDB demo \n\r");
	printf(" 2) Log demo - new log\n\r");
	printf(" 3) Log demo - resume log\n\r");
	printf(" 4) Persistent List demo\n\r");

	while(UTIL_DbguGetIntegerMinMax(&selection, 1, 4) == 0)
	{
	}

	switch(selection)
	{
		case 1:
			ParamsDBdemo();
			break;

		case 2:
			LogDemo(TRUE);
			offerMoreTests = FALSE;
			break;

		case 3:
			LogDemo(FALSE);
			offerMoreTests = FALSE;
			break;

		case 4:
			PListDemo();
			break;
		default:
			break;
	}

	return offerMoreTests;
}

void taskMain()
{
	int ret;

	// Default time is 1st of January 2015, 00:00:00 UTC
	Time defaulttime = { .seconds = 0,
							.minutes = 0,
							.hours = 0,
							.day = 5,
							.date = 1,
							.month = 1,
							.year = 15 };

	int choice;
	Boolean offerMoreTests = FALSE;

	WDT_startWatchdogKickTask(10 / portTICK_RATE_MS, FALSE);

	printf("\n\nDemo applications for ISIS OBC Mission Support Library built on %s at %s\n", __DATE__, __TIME__);
	printf("\nDemo applications use:\n");
	printf("* MS lib version %s.%s.%s built on %s at %s\n",
			MissionSupportVersionMajor, MissionSupportVersionMinor, MissionSupportVersionRevision,
			MissionSupportCompileDate, MissionSupportCompileTime);
	printf("* HAL lib version %s.%s.%s built on %s at %s\n", HalVersionMajor, HalVersionMinor, HalVersionRevision,
			HalCompileDate, HalCompileTime);

	// Initialize SPI 0
	if(SPI_start(bus0_spi, slave0_spi) != 0)
	{
		printf("Error initializing SPI bus 0!\n");
	}

	// Start time
	if(Time_start(&defaulttime, 60) != 0)
	{
		printf("Error starting iOBC time!\n");
	}

	// Start FRAM
	if(FRAM_start() != 0)
	{
		printf("Error starting FRAM driver!\n");
	}

	// Mount SD card 0

	if( HCC_MEM_SUCCESS != hcc_mem_init() )
	{
		printf("Error (%d) initializing filesystem memory!\n", f_getlasterror());
		while(1);
	}

	if(F_NO_ERROR != fs_init())
	{
		printf("Error (%d) initializing filesystem!\n", f_getlasterror());
		while(1);
	}

	if( F_NO_ERROR != f_enterFS() )
	{
		printf("Error (%d) entering filesystem!\n", f_getlasterror());
		while(1);
	}

	ret = f_initvolume( 0, atmel_mcipdc_initfunc, 0 );

	if( F_ERR_NOTFORMATTED == ret )
	{
		printf("Filesystem not formatted!");
		while(1);
	}
	else if( F_NO_ERROR != ret )
	{
		printf("Error (%d) initializing volume!", f_getlasterror());
		while(1);
	}

	f_releaseFS( );

	while(1) {
		LED_toggle(led_1);

		offerMoreTests = selectAndExecuteTest();

		if(offerMoreTests != FALSE) {
			// Not all tests will actually exit, so we may not reach here.
			// Even when we do, its good to be careful not to simultaneously run too many tests.
			// Instead, reboot between the tests.
			// In some cases, you must select 0 here to allow the test-tasks to do their work.
			printf("Perform more tests? (1=Yes, 0=No): \n\r");
			// No WatchDog resets here: The DBGU driver resets the WatchDog while it waits for user to enter characters.
			while(UTIL_DbguGetIntegerMinMax(&choice, 0, 1) == 0);
			if(choice == 0) {
				break;
			}
		}
		else {
			break;
		}
	}

	// Suspend itself.
	//vTaskSuspend(NULL);

	while(1) {
		LED_toggle(led_1);
		vTaskDelay(500);
	}

}

int main()
{
	unsigned int i;
	xTaskHandle taskMainHandle;

	TRACE_CONFIGURE_ISP(DBGU_STANDARD, 115200, BOARD_MCK);
	// Enable the Instruction cache of the ARM9 core. Keep the MMU and Data Cache disabled.
	CP15_Enable_I_Cache();

	LED_start();

	// The actual watchdog is already started, this only initializes the watchdog-kick interface.
	WDT_start();

	LED_wave(1);
	LED_waveReverse(1);
	LED_wave(1);
	LED_waveReverse(1);

	MAIN_TRACE_DEBUG("\t main: Starting main task.. \n\r");
	xTaskGenericCreate(taskMain, (const signed char*)"taskMain", 1024, NULL, configMAX_PRIORITIES-2, &taskMainHandle, NULL, NULL);

	MAIN_TRACE_DEBUG("\t main: Starting scheduler.. \n\r");
	vTaskStartScheduler();

	// This part should never be reached.
	MAIN_TRACE_DEBUG("\t main: Unexpected end of scheduling \n\r");

	//Flash some LEDs for about 100 seconds
	for (i=0; i < 2500; i++)
	{
		LED_wave(1);
		MAIN_TRACE_DEBUG("MAIN: STILL ALIVE %d\n\r", i);
	}
	exit(0);
}
