/*
 * common.c
 *
 *  Created on: 21/06/2024
 *      Author: Luis
 */

#include <stdlib.h>
#include <string.h>

#include "Labsat/uart_common.h"

#define CMD_START "<cmd>"
#define CMD_END "</cmd>"

#define RSP_START "<rsp>"
#define RSP_END "</rsp>"

#define MAX_RETRIES 1

typedef struct uart_read_params {
	UARTbus bus;
	eps_command command_info;
	unsigned char* buffer;
	int error_code;
} uart_read_params;

void post_read_callback(SystemContext context, xSemaphoreHandle sem) {
	(void) context;
	(void) sem;
	printf("\n\r[UART READ] Completed\n\r");
}

void free_resources(unsigned char* write_buffer, unsigned char* read_buffer) {
	if (write_buffer != NULL) {
		free(write_buffer);
	}
	if (read_buffer != NULL) {
		free(read_buffer);
	}
	xSemaphoreGive(uart_semaphore);
}

int validate_response_format(eps_command command_info,
		unsigned char* read_buffer) {
	int rsp_start_index, rsp_end_index;
	unsigned char* rsp_start = (unsigned char*) strstr((char*) read_buffer,
	RSP_START);
	unsigned char* rsp_end =
			(unsigned char*) strstr(
					(char*) (read_buffer + strlen(RSP_START)
							+ command_info.read_length), RSP_END);

	rsp_start_index = rsp_start - read_buffer;
	rsp_end_index = (rsp_end - read_buffer)
			- (strlen(RSP_START) + command_info.read_length);
	if (!(rsp_start_index == 0 && rsp_end_index == 0)) {
		return -1;
	}
	return 0;
}

int uart_eps_send_cmd(eps_command command_info, unsigned char* command,
		unsigned char* response) {
	unsigned char* read_buffer;
	unsigned char* write_buffer;
	int error_code;
	unsigned int i;
	unsigned int offset = 0;
	unsigned int total_write_length = command_info.write_length
			+ strlen(CMD_START) + strlen(CMD_END); // TODO change strlen to sizeof
	unsigned int total_read_length = command_info.read_length
			+ strlen(RSP_START) + strlen(RSP_END);
	UARTtransferStatus read_transfer_result;

	xSemaphoreTake(uart_semaphore, (portTickType )1);
	write_buffer = (unsigned char*) malloc(total_write_length); // TODO change to freertos malloc alternative
	if (write_buffer == NULL) {
		free_resources(write_buffer, read_buffer);
		return -1;
	}

	strcpy((char *) write_buffer + offset, CMD_START);
	offset += strlen(CMD_START);

	memcpy(write_buffer + offset, command, command_info.write_length);
	offset += command_info.write_length;

	strcpy((char *) write_buffer + offset, CMD_END);

#ifndef FLIGHT_VERSION
	printf("\n\r[UART WRITE] ");
	for (i = 0; i < total_write_length; i++) {
		printf("%02x", write_buffer[i]);
	}
	printf("\n\r");
#endif

	read_buffer = (unsigned char*) malloc(total_read_length);
	if (read_buffer == NULL) {
		free_resources(write_buffer, read_buffer);
		return -1;
	}
	memset(read_buffer, 0, total_read_length);
	// TODO limpiar antes de leer
	UARTgenericTransfer read_transfer = { .bus = DEFAULT_UART, .direction =
			read_uartDir, .readData = read_buffer,
			.readSize = total_read_length, .postTransferDelay = 0, .result =
					&read_transfer_result, .semaphore = uart_semaphore,
			.callback = post_read_callback };
	error_code = UART_queueTransfer(&read_transfer);
	if (error_code != 0) {
		free_resources(write_buffer, read_buffer);
		return error_code;
	}

	error_code = UART_write(DEFAULT_UART, write_buffer, total_write_length);
	if (error_code != 0) {
		free_resources(write_buffer, read_buffer);
		return error_code;
	}

	vTaskDelay(UART_RS_WAIT_TIME);
	int retry_counter = 0;
	while (!(retry_counter == MAX_RETRIES || read_transfer_result == 0)) {
		if (read_transfer_result != 0 && retry_counter) {
			printf("\n\r[READ TIMEOUT] Resending command...\n\r");
			error_code = UART_write(DEFAULT_UART, write_buffer,
					total_write_length);
			if (error_code != 0) {
				free_resources(write_buffer, read_buffer);
				return error_code;
			}
			vTaskDelay(UART_RS_WAIT_TIME);
		}
		retry_counter++;
		if (retry_counter == MAX_RETRIES && read_transfer_result != 0) {
			printf("\n\r[READ TIMEOUT] Skipping command...\n\r");
			error_code = stop_uart();
			if (error_code != 0) {
				printf("\n\r[READ TIMEOUT] Error stopping UART\n\r");
				free_resources(write_buffer, read_buffer);
				return error_code;
			}
			error_code = start_uart();
			if (error_code != 0) {
				printf("\n\r[READ TIMEOUT] Error restarting UART\n\r");
				free_resources(write_buffer, read_buffer);
				return error_code;
			}
			printf("\n\r[READ TIMEOUT] Command skipped\n\r");
			memset(response, 0, command_info.read_length);
			free_resources(write_buffer, read_buffer);
			return 1;
		}
	}

	printf("\n\r[UART READ] ");
	for (i = 0; i < total_read_length; i++) {
		printf("%02x", read_buffer[i]);
	}
	printf("\n\r");

	if (validate_response_format(command_info, read_buffer) != 0) {
		printf("\n\r[UART READ] Unexpected format\n\r");
		free_resources(write_buffer, read_buffer);
		return -1;
	}

	memcpy(response, read_buffer + strlen(RSP_START), command_info.read_length);
	free_resources(write_buffer, read_buffer);
	return error_code;
}



