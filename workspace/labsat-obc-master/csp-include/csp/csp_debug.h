#pragma once

#include <csp_autoconfig.h>
#include <inttypes.h>
#include "LogManager.h"

/**
 * NEW DEBUG API:
 * 
 * Based on counters, and error numbers.
 * This gets rid of a lot of verbose debugging strings while
 * still maintaining the same level of debug capabilities.
 * 
 * NOTE: We choose to ignore atomic access to the counters right now.
 *   1) Most of the access to these happens single threaded (router task) or within ISR (driver RX)
 *   2) Having accurate error counters is NOT a priority. They are only there for debugging purposes.
 *   3) Not all compilers have support for <stdatomic.h> yet.
 * 
 */

/** Error counters */
extern uint8_t csp_dbg_buffer_out;
extern uint8_t csp_dbg_conn_out;
extern uint8_t csp_dbg_conn_ovf;
extern uint8_t csp_dbg_conn_noroute;
extern uint8_t csp_dbg_inval_reply;

/* Central errno */
extern uint8_t csp_dbg_errno;
#define CSP_DBG_ERR_CORRUPT_BUFFER 1
#define CSP_DBG_ERR_MTU_EXCEEDED 2
#define CSP_DBG_ERR_ALREADY_FREE 3
#define CSP_DBG_ERR_REFCOUNT 4
#define CSP_DBG_ERR_INVALID_RTABLE_ENTRY 6
#define CSP_DBG_ERR_UNSUPPORTED 7
#define CSP_DBG_ERR_INVALID_BIND_PORT 8
#define CSP_DBG_ERR_PORT_ALREADY_IN_USE 9
#define CSP_DBG_ERR_ALREADY_CLOSED 10
#define CSP_DBG_ERR_INVALID_POINTER 11
#define CSP_DBG_ERR_CLOCK_SET_FAIL 12

/* CAN protocol specific errno */
extern uint8_t csp_dbg_can_errno;
#define CSP_DBG_CAN_ERR_FRAME_LOST 1
#define CSP_DBG_CAN_ERR_RX_OVF 2
#define CSP_DBG_CAN_ERR_RX_OUT 3
#define CSP_DBG_CAN_ERR_SHORT_BEGIN 4
#define CSP_DBG_CAN_ERR_INCOMPLETE 5
#define CSP_DBG_CAN_ERR_UNKNOWN 6

/* Toogle flags for rdp and packet print */
extern uint8_t csp_dbg_rdp_print;
extern uint8_t csp_dbg_packet_print;

/* Compile time disable all printout from CSP */
// Arreglado por Alejandro
#if (CSP_ENABLE_CSP_PRINT)
	#define csp_print(...)      UPLOG_INFO(__VA_ARGS__);
	#define csp_print_func(...) UPLOG_INFO(__VA_ARGS__);
	#define csp_rdp_error(format, ...) { if (csp_dbg_rdp_print >= 1) { UPLOG_ERR("\033[31m" format "\033[0m", ##__VA_ARGS__); }}
	#define csp_rdp_protocol(format, ...) { if (csp_dbg_rdp_print >= 2) { UPLOG_INFO("\033[34m" format "\033[0m", ##__VA_ARGS__); }}
	#define csp_print_packet(format, ...) { if (csp_dbg_packet_print >= 1) { UPLOG_DEBUG("\033[32m" format "\033[0m", ##__VA_ARGS__); }}
#else
	#define csp_print(...)
	#define csp_print_func(...)
#endif

