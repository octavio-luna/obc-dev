/*
 * interfaces.c
 *
 *  Created on: 28/07/2024
 *      Author: Luis
 */

#include <satellite-subsystems/isismepsv2_ivid7_piu.h>
#include "Labsat/interfaces.h"
#include "Labsat/uart_common.h"
#include <hal/Drivers/UART.h>
#include <freertos/task.h>

typedef struct interface_state {
	enum Interfaces interface;
	boolean state;
} interface_state;

xSemaphoreHandle interfaces_semaphore;
interface_state interface_state_table[] = { { .interface = I2C, .state = TRUE },
		{ .interface = UART, .state = TRUE } };

enum Interfaces get_current_interface() {
	int i;
	enum Interfaces result = -1;
	xSemaphoreTake(interfaces_semaphore, (portTickType )1);
	for (i = 0; i < 2; i++) {
		if (interface_state_table[i].state == TRUE
				&& result == (enum Interfaces) -1) {
			result = interface_state_table[i].interface;
		}
	}
	xSemaphoreGive(interfaces_semaphore);
	return result;
}

void set_interface_state(enum Interfaces interface, boolean state) {
	int i;
	xSemaphoreTake(interfaces_semaphore, (portTickType )1);
	for (i = 0; i < 2; i++) {
		if (interface_state_table[i].interface == interface) {
			interface_state_table[i].state = state;
		}
	}
	xSemaphoreGive(interfaces_semaphore);
}

boolean get_interface_state(enum Interfaces i) {
	int idx;
	xSemaphoreTake(interfaces_semaphore, (portTickType )1);
	boolean state = state;
	for (idx = 0; idx < 2; idx++) {
		if (interface_state_table[idx].interface == i) {
			state = interface_state_table[idx].state;
		}
	}

	xSemaphoreGive(interfaces_semaphore);
	return state;
}

void enable_interface(enum Interfaces i) {
	set_interface_state(i, TRUE);
}

void disable_interface(enum Interfaces i) {
	set_interface_state(i, FALSE);
}

int initialize_interfaces() {
	vSemaphoreCreateBinary(interfaces_semaphore);
	if (!interfaces_semaphore) {
		return -40;
	}
	xSemaphoreTake(interfaces_semaphore, (portTickType )1);
	int uart_error = initialize_uart();
	if (uart_error != 0) {
		printf("\t Error initializing UART: %d \n\r", uart_error);
		return uart_error;
	}
	ISISMEPSV2_IVID7_PIU_t subsystem[1];
	uint8_t i2c_error = I2C_start(200000, 10);

	if (i2c_error != 0) {
		printf("\nI2C init error %d\n\r", i2c_error);
	}
	subsystem[0].i2cAddr = 0x20;

	i2c_error = ISISMEPSV2_IVID7_PIU_Init(subsystem, 1);
	if (i2c_error == driver_error_reinit) {
		printf("\nISIS_EPS subsystem have already been initialised.\n\r");
		return i2c_error;
	} else if (i2c_error != driver_error_none) {
		printf("\nisismepsv2_ivid7_piu_Init(...) returned error %d! \n\r",
				i2c_error);
		return i2c_error;
	}

	if (i2c_error == 0 && uart_error == 0) {
		printf("\n Interfaces intialized successfully \n\r");
	}
	xSemaphoreGive(interfaces_semaphore);
	return 0;
}

void task_interface_manager(void* parameters) {
	(void) parameters;
	int error_code = initialize_interfaces();
	if (error_code != 0) {
		printf("\t Error initializing interfaces: %d \n\r", error_code);
		return;
	}
	while (1) {
		vTaskDelay(INTERFACES_CHECK_INTERVAL);
		if (get_interface_state(I2C) == FALSE) {
			enable_interface(I2C);
			printf("\n I2C reset \n\r");
		}
		if (get_interface_state(UART) == FALSE) {
			enable_interface(UART);
			printf("\n UART reset \n\r");
		}
	}
}

