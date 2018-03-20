
#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define STACK_SIZE (CONFIG_MAIN_TASK_STACK_SIZE)
#define TINYGO_HAS_STACKTRACE (0)
#define TINYGO_HAS_GOROUTINE_INFO (0)
#define TINYGO_CAN_RECOVER (0)

typedef TaskHandle_t goroutine_t;

#define GOROUTINE_GET() (xTaskGetCurrentTaskHandle())
#define GOROUTINE_GET_DEFERRED(g)       (pvTaskGetThreadLocalStoragePointer(g, 1))
#define GOROUTINE_SET_DEFERRED(g, val)  (vTaskSetThreadLocalStoragePointer(g, 1, val))
