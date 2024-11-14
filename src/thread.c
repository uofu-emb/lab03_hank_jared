#include <stdio.h>
#include <FreeRTOS.h>
#include <semphr.h>
#include <task.h>
#include <pico/stdlib.h>
#include <pico/multicore.h>
#include <pico/cyw43_arch.h>
#include "thread_helpers.h"

SemaphoreHandle_t semaphore;
TaskHandle_t first, second;

int counter = 0;
bool on = false;

void master_thread(void *params) {
    // Create the semaphore and tasks
    semaphore = xSemaphoreCreateCounting(1, 1);
    xTaskCreate(first_thread, "FirstThread",
                SIDE_TASK_STACK_SIZE, NULL, SIDE_TASK_PRIORITY, &first);
    xTaskCreate(second_thread, "SecondThread",
                SIDE_TASK_STACK_SIZE, NULL, SIDE_TASK_PRIORITY, &second);

    // Endless loop for task
    while (1) {
    }
}

// This task toggles the LED, prints, and increments the counter.
void first_thread(void *params) {
    while (1) {
        vTaskDelay(100);
        blink_led(&on, semaphore, portMAX_DELAY);
        print_counter(first, semaphore, &counter, portMAX_DELAY);
	}
}

// This task prints and increments the counter.
void second_thread(void *params) {
	while (1) {
        vTaskDelay(100);
        print_counter(second, semaphore, &counter, portMAX_DELAY);
	}
}