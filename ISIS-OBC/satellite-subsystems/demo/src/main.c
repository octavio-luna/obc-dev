/*
 * main.c
 *      Author: Akhil
 *     	Updated: 20/10/2023
 *     	Author: OBAR
 */

#include "Demos/isis_ants_demo.h"
#include "Demos/isis_ants2_demo.h"
#include "Demos/GomEPSdemo.h"
#include "Demos/IsisSPv2demo.h"
#include "Demos/IsisHSTXS_V2demo.h"
#include "Demos/isisRXSrevCdemo.h"
#include "Demos/IsisMTQv2demo.h"
#include "Demos/cspaceADCSdemo.h"
#include "Demos/ScsGeckoDemo.h"
#include "Demos/IsisTRXVUrevDdemo.h"
#include "Demos/IsisTRXVUrevCdemo.h"
#include "Demos/IsisTRXVUrevEdemo.h"
#include "Demos/IsisAOCSdemo.h"
#include <satellite-subsystems/version/version.h>

#include <at91/utility/exithandler.h>
#include <at91/commons.h>
#include <at91/utility/trace.h>
#include <at91/peripherals/cp15/cp15.h>
#include <at91/peripherals/pio/pio_it.h>
#include <Demos/isismeps_ivid5_pdu_demo.h>
#include <Demos/isismeps_ivid5_piu_demo.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>

#include <hal/Utility/util.h>
#include <hal/Timing/WatchDogTimer.h>
#include <hal/Drivers/LED.h>
#include <hal/Drivers/I2C.h>
#include <hal/Drivers/SPI.h>
#include <hal/boolean.h>
#include <hal/version/version.h>

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

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

	//Initialize the I2C
	int retValInt = I2C_start(200000, 10);
	if(retValInt != 0)
	{
		TRACE_FATAL("\n\r I2C_start_Master for demo: %d! \n\r", retValInt);
	}

	printf( "\n\r Select the device to be tested to perform: \n\r");
	printf("\t 1) TRXVU rev.C test \n\r");
	printf("\t 2) TRXVU rev.D test \n\r");
	printf("\t 3) TRXVU rev.E test \n\r");
	printf("\t 4) HSTxS V2 test \n\r");
	printf("\t 5) RXS rev.C test \n\r");
	printf("\t 6) AntS test \n\r");
	printf("\t 7) AntS Rev 2 test \n\r");
	printf("\t 8) Solar Panels V2 test \n\r");
	printf("\t 9) MTQv2 test \n\r");
	printf("\t 10) ISIS PDU iMEPS test \n\r");
	printf("\t 11) ISIS PIU iCEPS Test \n\r");
	printf("\t 12) ISIS AOCS test \n\r");
	printf("\t 13) Gecko Test \n\r");
	printf("\t 14) CubeSpace ADCS Test \n\r");
	printf("\t 15) GOM EPS Test \n\r");


	while(UTIL_DbguGetIntegerMinMax(&selection, 1, 15) == 0);

	switch(selection)
	{
        case 1:
            offerMoreTests = TRXVUrevCtest();
            break;
		case 2:
			offerMoreTests = TRXVUrevDtest();
			break;
		case 3:
			offerMoreTests = TRXVUrevEtest();
			break;
		case 4:
			offerMoreTests = IsisHSTxSV2demoMain();
			break;
		case 5:
			offerMoreTests = RXSrevCtest();
			break;
		case 6:
			offerMoreTests = AntStest();
			break;
        case 7:
            offerMoreTests = AntS2test();
            break;
		case 8:
			offerMoreTests = SolarPanelv2test();
			break;
		case 9:
			offerMoreTests = IsisMTQv2test();
			break;
		case 10:
			offerMoreTests = isismepsv2_ivid5_pdu__test();
			break;
		case 11:
			offerMoreTests = isismepsv2_ivid5_piu__test();
			break;
		case 12:
			offerMoreTests = isis_aocs_demo();
			break;
		case 13:
			offerMoreTests = GeckoTest();
			break;
		case 14:
			offerMoreTests = cspaceADCStest();
			break;
		case 15:
			offerMoreTests = GomEPStest();
			break;

		default:
			break;
	}

	return offerMoreTests;
}

void taskMain()
{
	Boolean offerMoreTests = FALSE;

	WDT_startWatchdogKickTask(10 / portTICK_RATE_MS, FALSE);

	while(1)
	{
		LED_toggle(led_1);

		offerMoreTests = selectAndExecuteTest();

		if(offerMoreTests == FALSE)
		{
			break;
		}
	}

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

	PIO_InitializeInterrupts(AT91C_AIC_PRIOR_LOWEST+4);

	printf("\n\nDemo applications for ISIS OBC Satellite Subsystems Library built on %s at %s\n", __DATE__, __TIME__);
	printf("\nDemo applications use:\n");
	printf("* Sat Subsys lib version %s.%s.%s built on %s at %s\n",
			SatelliteSubsystemsVersionMajor, SatelliteSubsystemsVersionMinor, SatelliteSubsystemsVersionRevision,
			SatelliteSubsystemsCompileDate, SatelliteSubsystemsCompileTime);
	printf("* HAL lib version %s.%s.%s built on %s at %s\n", HalVersionMajor, HalVersionMinor, HalVersionRevision,
			HalCompileDate, HalCompileTime);

	LED_wave(1);
	LED_waveReverse(1);
	LED_wave(1);
	LED_waveReverse(1);

	MAIN_TRACE_DEBUG("\t main: Starting main task.. \n\r");
	xTaskGenericCreate(taskMain, (const signed char*)"taskMain", 4096, NULL, configMAX_PRIORITIES-2, &taskMainHandle, NULL, NULL);

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
