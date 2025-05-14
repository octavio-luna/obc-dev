/*
 * uart_executor.h
 *
 *  Created on: 24/08/2024
 *      Author: Luis
 */

#ifndef UART_EXECUTOR_H_
#define UART_EXECUTOR_H_

#include <hal/Drivers/UART.h>
#include "Labsat/PowerManager.h"

#define DEFAULT_UART bus0_uart

#define MESSAGE_BUFFER_SIZE 100
#define REPONSE_BUFFER_SIZE 100

int uart_eps_send_cmd(eps_command command_info, unsigned char* command, unsigned char* response);


typedef struct uart_queue_msg {
	eps_command command_info;
	unsigned char command[MESSAGE_BUFFER_SIZE];
	void (*response_callback)(unsigned char response[MESSAGE_BUFFER_SIZE]);
} uart_queue_msg;

void uart_executor_task(void* parameters);
int request_uart_execution(uart_queue_msg* msg);


extern xSemaphoreHandle uart_semaphore;

// public
int start_uart();
int stop_uart();
int initialize_uart();


#endif /* UART_EXECUTOR_H_ */
