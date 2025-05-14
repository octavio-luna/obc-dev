#ifndef FREERTOS_VERSIN_COMPAT_H
#define FREERTOS_VERSIN_COMPAT_H

// FreeRTOS versions compatibility addons
#include<FreeRTOS.h>
#include<task.h>

// Some freertos old versions stupidly define a period as a rate:
// #define portTICK_RATE_MS ( ( portTickType ) 1000 / configTICK_RATE_HZ )
// We fix it here:
#define portTICK_PERIOD_MS ( ( portTickType ) 1000 / configTICK_RATE_HZ )

// Old freerts have different tye definitions
typedef xTaskHandle TaskHandle_t;

#endif
