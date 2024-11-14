#include <stdio.h>
#include <FreeRTOS.h>
#include <semphr.h>
#include <task.h>
#include <pico/stdlib.h>
#include <pico/multicore.h>
#include <pico/cyw43_arch.h>
#include <unity.h>
#include "unity_config.h"
#include "thread_helpers.h"

void setUp(void) {}
void tearDown(void) {}

void dummy_thread(void *args);
void left_thread(void *args);
void right_thread(void *args);

// Tests if the semaphore is actually blocked during
void test_blocked() {
    SemaphoreHandle_t semaphore = xSemaphoreCreateCounting(1, 1);
    TaskHandle_t dummy;
    int counter = 0;
    bool on = false;
    int status;

    // Create dummy task and take the semaphore
    xTaskCreate(dummy_thread, "DummyThread",
                SIDE_TASK_STACK_SIZE, NULL, SIDE_TASK_PRIORITY, &dummy);
    xSemaphoreTake(semaphore, portMAX_DELAY);

    // Send the dummy to take fromn the semaphore from print_counter. Should block resulting 
    // in fail return value.
    status = print_counter(dummy, semaphore, &counter, 100);
    TEST_ASSERT_EQUAL(0, status);

    // Send the dummy to take semaphore from blink_led. Should block resulting in fail return
    // value.
    status = blink_led(&on, semaphore, 100);
    TEST_ASSERT_EQUAL(0, status);
}

void test_unblocked() {
    SemaphoreHandle_t semaphore = xSemaphoreCreateCounting(1, 1);
    TaskHandle_t dummy;
    int counter = 0;
    bool on = false;
    int status;

    // Create dummy task and take the semaphore
    xTaskCreate(dummy_thread, "DummyThread",
                SIDE_TASK_STACK_SIZE, NULL, SIDE_TASK_PRIORITY, &dummy);

    status = print_counter(dummy, semaphore, &counter, 1000);
    TEST_ASSERT_EQUAL(1, status);
    TEST_ASSERT_EQUAL(1, counter);

    status = blink_led(&on, semaphore, 1000);
    TEST_ASSERT_EQUAL(1, status);
    TEST_ASSERT_EQUAL(1, on);
}

void test_deadlock() {
    SemaphoreHandle_t sema_one = xSemaphoreCreateCounting(1, 1);
    SemaphoreHandle_t sema_one = xSemaphoreCreateCounting(1, 1);
    TaskHandle_t dummy_one, dummy_two;
    int counter = 0;
    int status;

    // Create the second task and immediately suspend it
    xTaskCreate(dummy_thread, "dummy_two",
                SIDE_TASK_STACK_SIZE, NULL, SIDE_TASK_PRIORITY, &dummy_two);
    vTaskSuspend(dummy_two);

    // Create the first task and delay for the second task to suspend
    xTaskCreate(dummy_thread, "dummy_one",
                SIDE_TASK_STACK_SIZE, NULL, SIDE_TASK_PRIORITY, &dummy_one);
    vTaskDelay(3000);
    
}


void test_thread(void *args) {
    printf("Start tests\n");
    UNITY_BEGIN();
    RUN_TEST(test_blocked);
    RUN_TEST(test_unblocked);
    UNITY_END();
    sleep_ms(10000);
}

void dummy_thread(void *args) {
    while(1) {

    }
}

int main (void) {
    stdio_init_all();
    hard_assert(cyw43_arch_init() == PICO_OK);
    sleep_ms(10000); // Give time for TTY to attach.
    TaskHandle_t test;
    xTaskCreate(test_thread, "TestThread",
                MAIN_TASK_STACK_SIZE, NULL, MAIN_TASK_PRIORITY, &test);
    vTaskStartScheduler();
    return 0;

}
