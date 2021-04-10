//
// Created by Matthew Eshleman on 4/9/21.
//
#include "fauxThread.h"
#include <thread>

bool xTaskCreate(TaskFunction_t pxTaskCode, const char *pcName,
                 size_t usStackDepth,
                 TaskHandle_t *pxCreatedTask)
{
    (void)pcName;
    (void)usStackDepth;

    auto thread = new std::thread(pxTaskCode);
    *pxCreatedTask = static_cast<TaskHandle_t>(thread);
    return true;
}

void vTaskDelete(TaskHandle_t handle)
{
    auto thread = static_cast<std::thread*>(handle);
    if (thread != nullptr)
    {
        if (thread->joinable())
        {
            thread->join();
        }

        delete thread;
    }
}