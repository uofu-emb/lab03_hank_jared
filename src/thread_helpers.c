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

// Creates a deadlock with two semaphores. First thread takes a first and thread 2 takes b first. First thread then
// tries to grab b, but it is taken. The opposite happens for b. Since neither returns the threads both block 
// indefinitely.
int deadlock(DeadlockData *deadlock_data) {
    // Increment counter and take the semaphore
    deadlock_data->counter++;
    printf("%s incremented counter\n", deadlock_data->task_name);

    // First block
    xSemaphoreTake(deadlock_data->a, portMAX_DELAY);
    deadlock_data->counter++;
    printf("%s took first semaphore and incremented counter\n", deadlock_data->task_name);
    vTaskDelay(1000);
    printf("%s made it after delay\n", deadlock_data->task_name);

    // Second block
    xSemaphoreTake(deadlock_data->b, portMAX_DELAY);
    // Code should never reach here
    deadlock_data->counter++;
    printf("%s took second semaphore and incremented counter. Code shouldn't reach here.\n",
            deadlock_data->task_name);
    xSemaphoreGive(deadlock_data->b);
    // End second block

    xSemaphoreGive(deadlock_data->a);
    // End first block

    // Suspend the task if they get here, they won't tho.
    vTaskSuspend(deadlock_data->task);
}

// Any thread that enters will take the semaphore then start the loop over without returning the semaphore. This blocks
// this thread and any others going in.
int orphaned_lock(SemaphoreHandle_t semaphore, int *counter, TickType_t timeout) {
    if (xSemaphoreTake(semaphore, timeout) == pdFALSE) {
        return pdFALSE;
    }

    *counter = *counter + 1;
    if (*(counter) % 2) {
        return pdFALSE;
    }

    printf("Count %d\n", *counter);
    return xSemaphoreGive(semaphore);
}

// This function properly takes and gives the semaphore, resulting in no deadlocking.
int unorphaned_lock(SemaphoreHandle_t semaphore, int *counter, TickType_t timeout) {
        if (xSemaphoreTake(semaphore, timeout) == pdFALSE) {
            return pdFALSE;
        }

        *counter = *counter + 1;
        if (!(*counter % 2)) {
            printf("Count %d\n", *counter);
        }
        return xSemaphoreGive(semaphore);
}