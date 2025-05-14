/*
 * get_status.c
 *
 *  Created on: 17/08/2024
 *      Author: Luis
 */

#include "Labsat/interfaces.h"
#include <satellite-subsystems/isismepsv2_ivid7_piu.h>
#include <freertos/FreeRTOS.h>
#include "Labsat/uart_common.h"

uint8_t i2c_index = 0; // we have only 1 PIU, so PIU index is always 0. See sat subsystems docs.

const eps_command get_status_cmd = { .cmd = 0x06, .write_length = 4,
		.read_length = 5 };

static void _print_eps_respone(isismepsv2_ivid7_piu__replyheader_t* replyheader)
{
	printf("System Type Identifier: %u \n\r", replyheader->fields.stid);
	printf("Interface Version Identifier: %u \n\r", replyheader->fields.ivid);
	printf("Response Code: %u \n\r", replyheader->fields.rc);
	printf("Board Identifier: %u \n\r", replyheader->fields.bid);
	printf("Command error: %u \n\r", replyheader->fields.cmderr);
	printf("New flag: %u \n\r", replyheader->fields.new_flag);
	printf("\n\r");
}


void get_status_response_callback(unsigned char* response) {
	int i;
	printf("Get status callback\n\r");
	for (i = 0; i < get_status_cmd.read_length; i++) {
		printf("%02x", response[i]);
	}
	printf("\n\r");
}

int execute_get_status_i2c() {
	isismepsv2_ivid7_piu__getsystemstatus__from_t response;
	int error_code = isismepsv2_ivid7_piu__getsystemstatus(i2c_index,
			&response);
	if (error_code == 0) {
		_print_eps_respone(&(response.fields.reply_header));
	}

	return error_code;
}

int execute_get_status_uart() {
	int error_code;
	uart_queue_msg queue_msg = { .command_info = get_status_cmd, .command = {
			0x1A, 0x07, 0x06, 0x01 }, .response_callback =
			get_status_response_callback };
	error_code = request_uart_execution(&queue_msg);
	if (error_code != 0) {
		return error_code;
	}
	TRACE_INFO("[EPS Get Status] Executing command using UART");
	return 0;
}

int get_eps_status(unsigned int when, void* privData){
	int error_code;
	enum Interfaces current_interface = get_current_interface();
	switch (current_interface) {
	case I2C:
		TRACE_INFO("[EPS Get Status] Executing command using I2C");
		error_code = execute_get_status_i2c();
		break;
	case UART:
		TRACE_INFO("[EPS Get Status] Executing command using UART");
		error_code = execute_get_status_uart();
		break;
	default:
		TRACE_INFO("[EPS Get Status] ALL INTERFACES ARE DISABLED");
		return -1;
	}
	if (error_code != 0) {
		disable_interface(current_interface);
		TRACE_ERROR("Error executing EPS Get Status command: %d", error_code);
		return error_code;
	}
	return 0;
}
