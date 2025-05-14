#ifndef CSPMANAGER_H
#define CSPMANAGER_H

#include "ObcGlobals.h"

// Define which uart will be used for csp transfers.
// CSPManager will be the owner of this uart and will be listening for incompin csp_packets
// and will also be sending packets
#define CSP_UART_BUS "uart2"
// Define local usart CSP address. Connections to this host can be addresses to this address
#define CSP_LOCAL_UART_ADDR 1

// See AT91 files and hal/Drivers/UART.h for available options
#define CSP_UART_MODE AT91C_US_USMODE_NORMAL | AT91C_US_CLKS_CLOCK | AT91C_US_CHRL_8_BITS | AT91C_US_PAR_NONE | AT91C_US_OVER_16 | AT91C_US_NBSTOP_1_BIT
#define CSP_UART_RATE 115200
#define CSP_UART_TIMEGUARD 1   /* expresado en en bits */
#define CSP_UART_BUS_TYPE rs422_withTermination_uart /* options are rs232_uart, rs422_noTermination_uart, rs422_withTermination_uart */
#define CSP_UART_DEFAULTTIMEOUT CSP_UART_RATE*100/1000   /* Timeout value for RX in baudrate ticks, so timeoutSecs = value / baudrate. Timeout only starts counting after the first byte of the transfer has been received. If a timeout is specified it affects all read functions (UART_read, UART_writeRead, UART_queueTransfer). */


#define CSP_UART_RX_PRIO (configMAX_PRIORITIES-2)  /* this is high priority, though not highest */
#define CSP_STACK_DEPTH  configMINIMAL_STACK_SIZE   /* check if 1024 is ok */
#define CSP_RX_RINGBUF_COUNT 2 /* number of buffers in RX ring buffer */
#define CSP_BUF_SIZE 288 /* libcsp supports 256 UART pkts (see libcsp/doc/mtu.md) */

int CSPManagerInit(const char* ifname);
void CSPShowStatus();
// TODO: CSPManagerReinit();

// UART0: 3.3V TTL, LVCMOS as set in options sheet
//        (could have been configured as RS232 before fabrication, but no it is set it is.
// UART2: Reconfigurable via software, to support either RS232 (with RTS/CTS flow control)
//        or full-duplex RS422/485 differential bus.
#endif
