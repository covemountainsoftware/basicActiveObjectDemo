//
// Created by Matthew Eshleman on 4/10/21.
// A 'faux' RTOS Thread/Task, modeled after FreeRTOS,
// internally using standard portable C++11 components.

#ifndef FAUXTHREAD_H
#define FAUXTHREAD_H

#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*TaskFunction_t)(void);
typedef void* TaskHandle_t;

bool xTaskCreate(TaskFunction_t pxTaskCode, const char* pcName, size_t usStackDepth, TaskHandle_t* pxCreatedTask);
void vTaskDelete(TaskHandle_t handle);

#ifdef __cplusplus
}
#endif

#endif //FAUXTHREAD_H
