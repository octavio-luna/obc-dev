#include "../../csp_semaphore.h"

#include <FreeRTOS.h>
#include <semphr.h>
#include <freertos-version-compat.h>

#include <csp/csp_debug.h>
#include <csp/csp.h>

void csp_bin_sem_init(csp_bin_sem_t * sem) {
	vSemaphoreCreateBinary(*sem);
}

int csp_bin_sem_wait(csp_bin_sem_t * sem, unsigned int timeout) {

	if( timeout!=CSP_MAX_TIMEOUT ) timeout = timeout / portTICK_PERIOD_MS;
	return (xSemaphoreTake(*sem,timeout)==pdPASS) ? CSP_SEMAPHORE_OK : CSP_SEMAPHORE_ERROR;
}

int csp_bin_sem_post(csp_bin_sem_t * sem) {

	 return (xSemaphoreGive(*sem)==pdPASS) ? CSP_SEMAPHORE_OK : CSP_SEMAPHORE_ERROR;
}
