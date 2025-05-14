/*
 * uart_executor.c
 *
 *  Created on: 24/08/2024
 *      Author: Luis
 */

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include "Labsat/uart_common.h"

#define QUEUE_SIZE 5

xQueueHandle uart_queue_handle;

int init_executor() {
	uart_queue_handle = xQueueCreate(QUEUE_SIZE, sizeof(uart_queue_msg));
	if (uart_queue_handle == NULL) {
		return -1;
	}
	return 0;
}

void execute_command_from_queue(uart_queue_msg* message) {
	int error_code;
	unsigned char response[MESSAGE_BUFFER_SIZE];
	error_code = uart_eps_send_cmd(message->command_info, message->command, response);
	if (error_code != 0) {
		TRACE_ERROR("Error executing UART command: %d", error_code);
	} else {
		message->response_callback(response);
	}
}

void uart_executor_task(void* parameters) {
	(void) parameters;
	int error_code;
	uart_queue_msg message;
	error_code = init_executor();
	if (error_code != 0) {
		TRACE_ERROR("Error initializing queue");
		return;
	}
	while (1) {
		unsigned portBASE_TYPE result = xQueueReceive(uart_queue_handle, &message, portMAX_DELAY);
		if (result == pdPASS) {
			execute_command_from_queue(&message);
		} else {
			TRACE_DEBUG("No data in the UART queue");
		}
	}
}

int request_uart_execution(uart_queue_msg* msg) {
	unsigned portBASE_TYPE result = xQueueSendToFront(uart_queue_handle, msg, portMAX_DELAY);
	if (result == pdPASS) {
		return 0;
	} else {
		return -1;
	}
}
