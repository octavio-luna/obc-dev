#ifndef OBC_GLOBALS_H
#define OBC_GLOBALS_H

#include <freertos/FreeRTOS.h>

#define basic_STACK_DEPTH (configMINIMAL_STACK_SIZE*4) // the miminum is 1024
#define basic_TASK_PRIORITY (configMAX_PRIORITIES-3) // (configMAX_PRIORITIES is 5 so the maximum would be 4)

#ifndef pdMS_TO_TICKS
 #define pdMS_TO_TICKS(xTimeInMs) ( ( ((portTickType)xTimeInMs) * ((portTickType)configTICK_RATE_HZ) ) / ((portTickType)1000) )
#endif

/* #define FLIGHT_VERSION 1 */
#undef FLIGHT_VERSION

/* FLIGHT_VERSION=1 ********************************************************/
#ifdef FLIGHT_VERSION

/* FLIGHT_VERSION undefined => earth lab version ***************************/
#else 

	/* earth lab version can be compiled to run on the OBC or in the quemu emulated isis-obc machine */
	#define EMULATED_ENVIRONMENT 1

	#if EMULATED_ENVIRONMENT
		#define TICKS_TO_MS 4000
		#define CHECK_FREQUENCY 1
		#define UART_RS_WAIT_TIME 1
		#define INTERFACES_CHECK_INTERVAL 1
	#else
		#define TICKS_TO_MS (1000 / configTICK_RATE_HZ)
		// every 500ms
		#define CHECK_FREQUENCY 500 / TICKS_TO_MS
		#define UART_RS_WAIT_TIME 3000 / TICKS_TO_MS
		#define INTERFACES_CHECK_INTERVAL 3000 / TICKS_TO_MS
	#endif

#endif


#endif /* OBC_CONSTANTS_H_ */
