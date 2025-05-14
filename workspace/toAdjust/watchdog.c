/*
 * watchdog.c
 *
 *  Created on: 29/04/2024
 *      Author: Luis
 */

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <hal/Drivers/UART.h>
#include "Labsat/watchdog.h"
#include "Labsat/interfaces.h"
#include "Labsat/uart_common.h"
#include <stdlib.h>
#include <satellite-subsystems/isismepsv2_ivid7_piu.h>

#define min(a,b)  ( ( (a)<(b) ) ? (a) : (b) )

extern uint8_t i2c_index; // we have only 1 PIU, so PIU index is always 0. See sat subsystems docs.
static int lastEpsCommandSent = 0;

const eps_command watchdog_cmd = { .cmd = 0x06, .write_length = 4,
		.read_length = 5 };

void watchdog_response_callback(unsigned char* response) {
	TRACE_INFO("Watchdog restarted successfully");
}

int execute_watchdog_uart() {
	int error_code;
	uart_queue_msg queue_msg = { .command_info = watchdog_cmd, .command = {
			0x1A, 0x07, 0x06, 0x01 }, .response_callback =
			watchdog_response_callback };
	error_code = request_uart_execution(&queue_msg);
	return error_code;
}

int execute_watchdog_i2c() {
	isismepsv2_ivid7_piu__replyheader_t response;
	int error_code;
	error_code = isismepsv2_ivid7_piu__resetwatchdog(i2c_index, &response);
	if (error_code != 0) {
		TRACE_ERROR("Error sending EPS watchdog command: %d", error_code);
		return error_code;
	}
	TRACE_INFO("EPS Watchdog executed successfully using I2C");
	return 0;
}
//"${workspace_loc:/isis-obc-first-project/src/include/lib}"

int reset_eps_watchdog(unsigned int when, void* privData) {
	// prevent sending wd reset if ther was a recent successful command sent
	unsigned int secsSinceLast = min( when-lastEpsCommandSent , EPS_WATCHDOG_INTERVAL_SECS);
	unsigned int remaining = EPS_WATCHDOG_INTERVAL_SECS-secsSinceLast;
	if( remaining > EPS_WATCHDOG_CHECK_SECS ) return 0;

	// send wd signal to EPS
	int error_code;
	enum Interfaces current_interface = get_current_interface();
	switch (current_interface) {
	case I2C:
		TRACE_INFO("[EPS Watchdog] Executing command using I2C");
		error_code = execute_watchdog_i2c();
		break;
	case UART:
		TRACE_INFO("[EPS Watchdog] Executing command using UART");
		error_code = execute_watchdog_uart();
		break;
	default:
		TRACE_INFO("[EPS Watchdog] ALL INTERFACES ARE DISABLED");
		break;
	}
	if (error_code != 0) {
		disable_interface(current_interface);
		TRACE_ERROR("Error executing EPS watchdog command: %d\n\r", error_code);
	} else {
		lastEpsCommandSent = when;
	}
	return 0;
}
