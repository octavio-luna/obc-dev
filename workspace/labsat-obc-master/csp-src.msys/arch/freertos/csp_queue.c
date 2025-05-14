

#include <csp/arch/csp_queue.h>
#include <csp/csp.h>

#include <FreeRTOS.h>
#include <queue.h>
#include <freertos-version-compat.h>

// Our freertos version has no static queue creation
csp_queue_handle_t csp_queue_create(int length, size_t item_size) {
	return xQueueCreate(length, item_size);
}

int csp_queue_enqueue(csp_queue_handle_t handle, const void * value, uint32_t timeout) {
	if (timeout != CSP_MAX_TIMEOUT)
		timeout = timeout / portTICK_PERIOD_MS;
	if (xQueueSendToBack(handle, value, timeout) == pdPASS) {
		return CSP_QUEUE_OK;
	}
	return CSP_QUEUE_ERROR;
}

int csp_queue_enqueue_isr(csp_queue_handle_t handle, const void * value, int * task_woken) {
	if (xQueueSendToBackFromISR(handle, value, (portBASE_TYPE *)task_woken) == pdPASS) {
		return CSP_QUEUE_OK;
	}
	return CSP_QUEUE_ERROR;
}

int csp_queue_dequeue(csp_queue_handle_t handle, void * buf, uint32_t timeout) {
	if (timeout != CSP_MAX_TIMEOUT)
		timeout = timeout / portTICK_PERIOD_MS;
	if (xQueueReceive(handle, buf, timeout) == pdPASS) {
		return CSP_QUEUE_OK;
	}
	return CSP_QUEUE_ERROR;
}

int csp_queue_dequeue_isr(csp_queue_handle_t handle, void * buf, int * task_woken) {
	if (xQueueReceiveFromISR(handle, buf, (portBASE_TYPE *)task_woken) == pdPASS) {
		return CSP_QUEUE_OK;
	}
	return CSP_QUEUE_ERROR;
}

int csp_queue_size(csp_queue_handle_t handle) {
	return uxQueueMessagesWaiting(handle);
}

int csp_queue_size_isr(csp_queue_handle_t handle) {
	return uxQueueMessagesWaitingFromISR(handle);
}

int csp_queue_free(csp_queue_handle_t handle) {
	return uxQueueSpacesAvailable(handle);
}
