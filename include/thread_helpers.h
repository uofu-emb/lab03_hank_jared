#pragma once

#include <FreeRTOS.h>
#include <semphr.h>
#include <task.h>
#include <pico/stdlib.h>
#include <pico/multicore.h>
#include <pico/cyw43_arch.h>

#define MAIN_TASK_PRIORITY      ( tskIDLE_PRIORITY + 1UL )
#define MAIN_TASK_STACK_SIZE configMINIMAL_STACK_SIZE

#define SIDE_TASK_PRIORITY      ( tskIDLE_PRIORITY + 1UL )
#define SIDE_TASK_STACK_SIZE configMINIMAL_STACK_SIZE

typedef struct DeadlockData {
    SemaphoreHandle_t a;
    SemaphoreHandle_t b;
    TaskHandle_t task;
    char* task_name;
    int counter;
} DeadlockData;

typedef struct OrphanedLockData {
    SemaphoreHandle_t semaphore;
    int counter;
} OrphanedLockData;

// Thread handlers
void master_thread(void *params);
void first_thread(void *params);
void second_thread(void *params);

// Helpers
int print_counter(TaskHandle_t task, SemaphoreHandle_t semaphore, int *counter, 
    TickType_t timeout);
int blink_led(bool *on, SemaphoreHandle_t semaphore, TickType_t timeout);
int deadlock(DeadlockData *deadlock_data);
void orphaned_lock(OrphanedLockData *orphan_data);
void unorphaned_lock(OrphanedLockData *orphan_data);