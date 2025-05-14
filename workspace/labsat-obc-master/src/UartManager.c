#include <PowerManager.h>
#include <LogManager.h>
#include <UartManager.h>
// use string.c at91 light versions of posix functions
#include <string.h>

#include "misc.h"

void (*epsUartRXCallback)(char* packetBuf, unsigned int len, char complete);
void (*gpsUartRXCallback)(char* packetBuf, unsigned int len, char complete);
void (*camUartRXCallback)(char* packetBuf, unsigned int len, char complete);

int UartManagerInitEPS() {
	return UartManagerInit(	EPS_UART_BUS,epsUartRXCallback,EPS_RX_RINGBUF_COUNT,EPS_BUF_SIZE,0,
									EPS_UART_MODE,EPS_UART_RATE,EPS_UART_TIMEGUARD,EPS_UART_BUS_TYPE,
									EPS_UART_DEFAULTTIMEOUT,"<rsp>","</rsp>\r\n");
}
int UartManagerInitCAM() {
	return UartManagerInit(	CAM_UART_BUS,camUartRXCallback,CAM_RX_RINGBUF_COUNT,CAM_BUF_SIZE,0,
									CAM_UART_MODE,CAM_UART_RATE,CAM_UART_TIMEGUARD,CAM_UART_BUS_TYPE,
									CAM_UART_DEFAULTTIMEOUT,"@","\r");
}
int UartManagerInitGPS() {
	return UartManagerInit(	GPS_UART_BUS,gpsUartRXCallback,GPS_RX_RINGBUF_COUNT,GPS_BUF_SIZE,0,
									GPS_UART_MODE,GPS_UART_RATE,GPS_UART_TIMEGUARD,GPS_UART_BUS_TYPE,
									GPS_UART_DEFAULTTIMEOUT,"#","\n"); // chequear si hay una secuencia de fin o no en command and log manuial de hexagon
																				  // ver si conviene binario en vez de ascii, lo que cambia el ini y fin de mensaje
																				  // chequear msg max len
}


typedef struct {
	void (*rxCallback)(char* packetBuf, unsigned int len, char complete);
	char  **rxBufRing;
	xTaskHandle rxTaskHandle;
	UARTbus bus; // isis obc accepts bus0_uart and bus2_uart
	uint32_t rxBufCount;
	uint32_t rxBufSize;
	const char *iniSeq, *endSeq;
	char passAlsoIncompleteBuf;
	char go;
} uartContext;

uartContext *uartData[2] = {0,0}; // bus0_uart=0, bus2_uart=1


static void UartRxTask(void* param) {
	uartContext* uctx = (uartContext*)param; int n;
	uctx->rxBufRing = pvPortMalloc( uctx->rxBufCount * ( sizeof(char*) + uctx->rxBufSize ) );
	uctx->rxBufRing[0] = (char*)(uctx->rxBufRing) + sizeof(char*) * uctx->rxBufCount;
	for(n=1; n!=uctx->rxBufCount; ++n) uctx->rxBufRing[n] = uctx->rxBufRing[n-1] + uctx->rxBufSize;
	uint8_t* rxCursor = uctx->rxBufRing[0];
	uint32_t rxBytes = 0;
	size_t lini = strlen(uctx->iniSeq)-1;
	size_t lfin = strlen(uctx->endSeq)-1;
	size_t ll = lini;
	const char* needle = uctx->iniSeq;
	const char* needleLast = needle + ll;
	const char* q = needleLast;
	uint8_t rxBufIdx = 0;
	UPLOG_NOTICE("%s starting",__FUNCTION__);
	while( uctx->go ) {
		// Performs a blocking read sleeping this task until a byte is read.
		// This allows other tasks to execute while the transfer is made using DMA.
		if( (n=UART_read(uctx->bus,rxCursor,1))!=0 ) {
			UPLOG_ERR("%s error reading uart(%u): %d",__FUNCTION__,uctx->bus,n);
			vTaskDelay(pdMS_TO_TICKS(1000)); // wait 1 second after read error, it is too long, but to prevent multiple consecutive errors
			continue; // TODO: analizar errores y ver si necesita reinicializar o anular el servicio
		}
		++rxBytes;
		if( *rxCursor==*q ) {
			if( rxBytes>ll ) {
				const char* p = rxCursor;
				while( --q >= needle ) {
					--p;
					if( *p != *q ) { q = needleLast; break; }
				}
				if( q < needle ) {
					// needle found
					if( needle==uctx->iniSeq ) { // start of frame found! => now wait for the end of frame
						needle = uctx->endSeq;
						ll = lfin;
					} else { // end of frame found!
						uctx->rxCallback(uctx->rxBufRing[rxBufIdx],rxBytes-lfin-1,1 /*complete*/ );
						if( ++rxBufIdx==uctx->rxBufCount ) rxBufIdx = 0;
						needle = uctx->iniSeq;
						ll = lini;
					}
					// even if we found the initial sequence, we will not pass the initial and last sequences
					// to the upper level, so we reuse its buffer space
					rxCursor = uctx->rxBufRing[rxBufIdx];
					rxBytes = 0;
					q = needleLast = needle + ll;
				}
			}
		}
		if( rxBytes == uctx->rxBufSize ) { // out of buffer space
			if( needle==uctx->iniSeq ) { // bufer full and iniSeq not found!
				// we do not pass anything to upper level and start over on same buffer
			} else if( uctx->passAlsoIncompleteBuf ) {
				uctx->rxCallback(uctx->rxBufRing[rxBufIdx],rxBytes,0 /*incomplete*/ );
				if( ++rxBufIdx==uctx->rxBufCount ) rxBufIdx = 0;
			}
			rxCursor = uctx->rxBufRing[rxBufIdx];
			rxBytes = 0;
		}
	}
	vPortFree(uctx->rxBufRing);
	vTaskDelete(NULL);
}



int UartManagerInit(
	UARTbus bus,
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
) {
	if( bus<0 or bus>UART_BUS_COUNT ) { UPLOG_ERR("%s nonvalid bus: %d",__FUNCTION__,bus); return -1; }
	if( uartData[bus] ) closeUart(bus);
	uartContext *uctx = (uartContext*)pvPortMalloc(sizeof(uartContext));
	uctx->go = 1;
	uctx->passAlsoIncompleteBuf = passAlsoIncompleteBuf;
	uctx->rxCallback = rxCallback;
	uctx->rxBufCount = rxBufCount;
	uctx->rxBufSize = rxBufSize;
	uctx->rxBufRing = 0;

	UARTconfig uconf = { mode,baudrate,timeGuard,busType,defaultTimeout };
	int res = UART_start(uctx->bus, uconf);
	if( res!=0 ) {
		UPLOG_ERR("%s error starting hal uart driver: %s",__FUNCTION__,res);
		vPortFree(uctx);
		return res;
	}
	if( pdPASS!=xTaskCreate(
				UartRxTask,
				"UartRxTask",
				UART_STACK_DEPTH,  /* size of stack to allocate to task... rxCallback uses this stack! */
				uctx,             /* The parameter passed to task */
				UART_RX_PRIO, /* task priority */
				&(uctx->rxTaskHandle) ) ) 
	{
		UPLOG_ERR("%s: failed to create csp_uart_rx_task for uart bus: %d\n", __FUNCTION__,bus);
		vPortFree(uctx);
		return -3;
	}
	UPLOG_INFO("%s opened bus %d\n",__FUNCTION__,bus);
	uartData[uctx->bus] = uctx;

	return 0;
}


void closeUart(UARTbus bus) {  // bus0_uart=0, bus2_uart=1
	uartContext *uctx = uartData[bus];
	if( uctx==0 ) return;
	uartData[bus]=0;
	uctx->go = 0; // this signal the task to stop the loop
	UART_stop(bus);
	vPortFree( uctx );
}
