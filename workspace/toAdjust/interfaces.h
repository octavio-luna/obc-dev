/*
 * interfaces.h
 *
 *  Created on: 28/07/2024
 *      Author: Luis
 */

#ifndef INTERFACES_H_
#define INTERFACES_H_

enum Interfaces {
  UART,
  I2C
};

typedef unsigned int boolean;
#define TRUE		0xFFFFFFFF
#define FALSE		0

void enable_interface(enum Interfaces i);
void disable_interface(enum Interfaces i);
enum Interfaces get_current_interface();
boolean get_interface_state(enum Interfaces i);
int initialize_interfaces();
void task_interface_manager(void* parameters);

#endif /* INTERFACES_H_ */
