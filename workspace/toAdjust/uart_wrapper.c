/*
 * uart.c
 *
 *  Created on: 25/06/2024
 *      Author: Luis
 */

#include "Labsat/uart_common.h"


xSemaphoreHandle uart_semaphore;

int start_uart() {
	UARTconfig config = { AT91C_US_USMODE_NORMAL | AT91C_US_CLKS_CLOCK
			| AT91C_US_CHRL_8_BITS | AT91C_US_PAR_NONE | AT91C_US_OVER_16
			| AT91C_US_NBSTOP_1_BIT, 115200, 1, rs232_uart, 0xFFFF };
	return UART_start(DEFAULT_UART, config);
}

int stop_uart() {
	return UART_stop(DEFAULT_UART);
}

int initialize_uart() {
	vSemaphoreCreateBinary(uart_semaphore);
	if (!uart_semaphore) {
		return -40;
	}
	return start_uart();
}
