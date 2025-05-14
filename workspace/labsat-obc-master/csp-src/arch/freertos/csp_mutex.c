#include <csp/csp.h>
#include <FreeRTOS.h>
#include <semphr.h>

#if (CSP_FREERTOS)
static xSemaphoreHandle usartMutex = 0;

void csp_usart_lock(void * driver_data) {
	if( usartMutex==0 ) usartMutex = xSemaphoreCreateMutex();
	xSemaphoreTake(usartMutex,portMAX_DELAY);
}

void csp_usart_unlock(void * driver_data) {
	xSemaphoreGive(usartMutex);
}
#else
	In this file only compiling for freertos
#endif
