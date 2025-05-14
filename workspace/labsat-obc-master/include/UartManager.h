#ifndef UARTMANAGER_H
#define UARTMANAGER_H

#include "ObcGlobals.h"

// FreeRTOS includes
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
// HAL includes
#include <hal/Drivers/UART.h>

#include<stdint.h>

#define UART_STACK_DEPTH configMINIMAL_STACK_SIZE*4   /* check if 1024 is ok */
#define UART_RX_PRIO (configMAX_PRIORITIES-2)  /* this is high priority, though not highest */

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// UART definitions for UART usage for different modules
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// EPS UART definitions
#define EPS_UART_BUS bus0_uart
#define EPS_GPIO_SELECT 0x00
// See at91/boards/ISIS_OBC_G20/at91sam9g20/AT91SAM9G20.h, hal/Drivers/UART.h
// and other at91 files for available options
#define EPS_UART_MODE AT91C_US_USMODE_NORMAL | AT91C_US_CLKS_CLOCK | AT91C_US_CHRL_8_BITS | AT91C_US_PAR_NONE | AT91C_US_NBSTOP_1_BIT | AT91C_US_OVER_16
#define EPS_UART_RATE 115200
#define EPS_UART_TIMEGUARD 1   /* expresado en en bits */
#define EPS_UART_BUS_TYPE 0 /* this is LVCMOS type in our case for UART0 */
#define EPS_RX_RINGBUF_COUNT 2 /* number of buffers in RX ring buffer */
#define EPS_BUF_SIZE sizeof(commandRespData) + 32 /* a la respuesta se le agrega <rsp>, </rsp> y <CR><LF> */
/* Timeout value for RX in baudrate ticks, so timeoutSecs=(value/EPS_UART_RATE). Timeout only starts counting after the first byte of the transfer has been received. If a timeout is specified it affects all read functions (UART_read, UART_writeRead, UART_queueTransfer). */
#define EPS_UART_DEFAULTTIMEOUT ((EPS_BUF_SIZE*8)+(20*EPS_UART_RATE)/1000)

/////////////////////////////////////////////////////////////////////////////
// GPS UART definitions
#define GPS_UART_BUS bus0_uart
#define GPS_GPIO_SELECT 0x01
// See at91/boards/ISIS_OBC_G20/at91sam9g20/AT91SAM9G20.h, hal/Drivers/UART.h
// and other at91 files for available options
#define GPS_UART_MODE AT91C_US_USMODE_NORMAL | AT91C_US_CLKS_CLOCK | AT91C_US_CHRL_8_BITS | AT91C_US_PAR_NONE | AT91C_US_NBSTOP_1_BIT | AT91C_US_OVER_16
#define GPS_UART_RATE 9600 /* supports much higher rate, recommended minimum is 230400, but needs to be set up before using this rate */
#define GPS_UART_TIMEGUARD 1   /* expresado en en bits */
#define GPS_UART_BUS_TYPE 0 /* this is LVCMOS type in our case for UART0 */
#define GPS_RX_RINGBUF_COUNT 2 /* number of buffers in RX ring buffer */
#define GPS_BUF_SIZE 384 /* TODO: chequear en el manual */
/* Timeout value for RX in baudrate ticks, so timeoutSecs=(value/EPS_UART_RATE). Timeout only starts counting after the first byte of the transfer has been received. If a timeout is specified it affects all read functions (UART_read, UART_writeRead, UART_queueTransfer). */
#define GPS_UART_DEFAULTTIMEOUT ((GPS_BUF_SIZE*8)+(10*GPS_UART_RATE)/1000)

/////////////////////////////////////////////////////////////////////////////
// KISS CAMERA UART definitions
#define CAM_UART_BUS bus0_uart
#define CAM1_GPIO_SELECT 0x10
#define CAM2_GPIO_SELECT 0x11
// See at91/boards/ISIS_OBC_G20/at91sam9g20/AT91SAM9G20.h, hal/Drivers/UART.h
// and other at91 files for available options
#define CAM_UART_MODE AT91C_US_USMODE_NORMAL | AT91C_US_CLKS_CLOCK | AT91C_US_CHRL_8_BITS | AT91C_US_PAR_NONE | AT91C_US_NBSTOP_1_BIT | AT91C_US_OVER_16
#define CAM_UART_RATE 115200 
#define CAM_UART_TIMEGUARD 1   /* expresado en en bits */
#define CAM_UART_BUS_TYPE 0 /* this is LVCMOS type in our case for UART0 */
#define CAM_RX_RINGBUF_COUNT 2 /* number of buffers in RX ring buffer */
#define CAM_BUF_SIZE 650 /* max msg len is for download image line */
/* Timeout value for RX in baudrate ticks, so timeoutSecs=(value/EPS_UART_RATE). Timeout only starts counting after the first byte of the transfer has been received. If a timeout is specified it affects all read functions (UART_read, UART_writeRead, UART_queueTransfer). */
#define GPS_UART_DEFAULTTIMEOUT ((CAM_BUF_SIZE*8)+(10*CAM_UART_RATE)/1000)


// Next function creates a task for continuosly reading uart, detecting start
// and end of frame, and calling rxCallback on each incoming frame.
// It can be called for uart0 or uart2 buses. If a uart bus is contrlled
// by another manager (i.e. CSPManager) do not call this initializer for that bus.
int UartManagerInit( const char* ifname,
   void (*rxCallback)(char* packetBuf, unsigned int len, char complete),
   uint32_t rxBufCount,
   uint32_t rxBufSize,
   char passAlsoIncompleteBuf,
	uint32_t mode,
	uint32_t baudrate,
	uint8_t timeGuard,
	UART2busType busType,
	uint16_t defaultTimeout,
	const char *iniSeq,
	const char *endSeq
);

void closeUart(UARTbus bus);  // bus0_uart=0, bus2_uart=1

#endif
