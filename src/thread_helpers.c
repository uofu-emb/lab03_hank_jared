#include <stdio.h>
#include <FreeRTOS.h>
#include <semphr.h>
#include <task.h>
#include <pico/stdlib.h>
#include <pico/multicore.h>
#include <pico/cyw43_arch.h>
#include "thread_helpers.h"

// Locks print statment in a semaphore and prints out thread details. Returns semaphore give
// status. 1 = Returned sucessfully, 0 = error.
int print_counter(TaskHandle_t task, SemaphoreHandle_t semaphore, int *counter, 
                    TickType_t timeout) {
    TaskStatus_t task_details;

    // Get task info and save the name.
    vTaskGetInfo(task, &task_details, pdTRUE, eInvalid);
    char* name = task_details.pcTaskName;

    // Take a semaphore, print the name, and increase the counter.
    if (xSemaphoreTake(semaphore, timeout) == pdFALSE) {
        return pdFALSE;
    }
    printf("hello world from %s Count %d\n", name, *counter);
    *counter = *counter + 1;

    // Return SemaphoreGives status
    return xSemaphoreGive(semaphore);
}

// Toggles the LED with a semaphore lock. Returns semaphore give status.
int blink_led(bool *on, SemaphoreHandle_t semaphore, TickType_t timeout) {
    // Take semaphore, toggle value of on and push it to the LED pin
    if(xSemaphoreTake(semaphore, timeout) == pdFALSE) {
        return pdFALSE;
    }
    *on = !*on;
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, *on);
    
    // Return SemaphoreGive status
    return xSemaphoreGive(semaphore);
}

int deadlock(DeadlockData *deadlock_data) {
    // Increment counter and take the semaphore
    deadlock_data->counter++;
    printf("%s incremented counter\n", deadlock_data->task_name);

    // First block
    xSemaphoreTake(deadlock_data->a, portMAX_DELAY);
    deadlock_data->counter++;
    printf("%s took first semaphore and incremented counter\n", deadlock_data->task_name);
    vTaskDelay(1000);
    printf("%s is after delay\n", deadlock_data->task_name);

    // Second block
    xSemaphoreTake(deadlock_data->b, portMAX_DELAY);
    // Code should never reach here
    deadlock_data->counter++;
    printf("%s took second semaphore and incremented counter. Code shouldn't reach here\n",
            deadlock_data->task_name);
    xSemaphoreGive(deadlock_data->b);
    // End second block

    xSemaphoreGive(deadlock_data->a);
    // End first block

    vTaskSuspend(deadlock_data->task);
}

void orphaned_lock(OrphanedLockData *orphan_data)
{
    while (1) {
        xSemaphoreTake(orphan_data->semaphore, portMAX_DELAY);
        orphan_data->counter++;
        if (orphan_data->counter % 2) {
            continue;
        }
        printk("Count %d\n", orphan_data->counter);
        xSemaphoreGive(orphan_data->semaphore);
    }
}

void unorphaned_lock(OrphanedLockData *orphan_data)
{
    while (1) {
        xSemaphoreTake(orphan_data->semaphore, portMAX_DELAY);
        orphan_data->counter++;
        if (orphan_data->counter % 2) {
            xSemaphoreGive(orphan_data->semaphore);
            continue;
        }
        printk("Count %d\n", orphan_data->counter);
        xSemaphoreGive(orphan_data->semaphore);
    }
}