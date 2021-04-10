//
// Created by Matthew Eshleman on 4/9/21.
// A 'faux' RTOS Queue, modeled after FreeRTOS,
// internally using standard portable C++11 components.
//

#ifndef FAUXQUEUE_H
#define FAUXQUEUE_H

#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void* QueueHandle_t;

QueueHandle_t xQueueCreate(size_t uxQueueLength, size_t uxItemSize);
void vQueueDelete( QueueHandle_t xQueue );
bool xQueueSendToBack(QueueHandle_t xQueue, const void* pvItemToQueue);
bool xQueueSendToFront(QueueHandle_t xQueue, const void* pvItemToQueue);
bool xQueueReceive(QueueHandle_t xQueue, void *pvBuffer);
size_t uxQueueMessagesWaiting(const QueueHandle_t xQueue);

#ifdef __cplusplus
}
#endif
#endif //FAUXQUEUE_H
