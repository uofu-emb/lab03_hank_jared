#include <stdio.h>
#include <FreeRTOS.h>
#include <semphr.h>
#include <task.h>
#include <pico/stdlib.h>
#include <pico/multicore.h>
#include <pico/cyw43_arch.h>
#include "thread_helpers.h"

//SemaphoreHandle_t semaphore;

int counter = 0;
bool on = false;

// void second_thread(void *params) {
// 	while (1) {
//         vTaskDelay(100);
        
//         // Critical section
//         xSemaphoreTake(semaphore, portMAX_DELAY);
//         counter += counter + 1;
// 		printf("hello world from %s! Count %d\n", "second", counter);
//         xSemaphoreGive(semaphore);
// 	}
// }

// void first_thread(void *params) {
//     int sema;
// 	while (1) {
//         cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, on);
//         vTaskDelay(100);

//         // Critical section
//         xSemaphoreTake(semaphore, 100);
// 		printf("hello world from %s! Count %d\n", "first", counter++);
//         sema = xSemaphoreGive(semaphore);

//         on = !on;
// 	}
// }

void master_thread(void *params) {
    // semaphore = xSemaphoreCreateCounting(1, 1);
    // xTaskCreate(first_thread, "FirstThread",
    //             SIDE_TASK_STACK_SIZE, NULL, SIDE_TASK_PRIORITY, NULL);
    // xTaskCreate(second_thread, "SecondThread",
    //             SIDE_TASK_STACK_SIZE, NULL, SIDE_TASK_PRIORITY, NULL);
    while (1) {
        vTaskDelay(5000);
        printf("hello world\n");
    }
}