#include <stdio.h>
#include <FreeRTOS.h>
#include <semphr.h>
#include <task.h>
#include <pico/stdlib.h>
#include <pico/multicore.h>
#include <pico/cyw43_arch.h>

int print_counter(TaskHandle_t task, SemaphoreHandle_t semaphore, int *counter) {
    TaskStatus_t task_details;

    vTaskGetInfo(task, &task_details, pdTRUE, eInvalid);
    char* name = task_details.pcTaskName;

    xSemaphoreTake(semaphore, portMAX_DELAY);
    printf("name = %s\n", name);
    *counter =+ 1;


    return xSemaphoreGive(semaphore);
}