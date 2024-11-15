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

// Tests if both print_counter and blink_led use a semaphore properly
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

// Tests the deadlock function
void test_deadlock() {
    SemaphoreHandle_t sema_one = xSemaphoreCreateCounting(1, 1);
    SemaphoreHandle_t sema_two = xSemaphoreCreateCounting(1, 1);
    TaskHandle_t dummy_one, dummy_two;
    TaskStatus_t first_status, second_status;
    int counter = 0;

    // Set up args for the deadlock method
    DeadlockData first = {sema_one, sema_two, dummy_one, "dummy_one", 0};
    DeadlockData second = {sema_two, sema_one, dummy_two, "dummy_two", 10};

    // Create the tasks
    printf("Creating deadlock tasks\n");
    xTaskCreate(deadlock, "dummy_one",
                SIDE_TASK_STACK_SIZE, &first, SIDE_TASK_PRIORITY, &dummy_one);
    xTaskCreate(deadlock, "dummy_two",
                SIDE_TASK_STACK_SIZE, &second, SIDE_TASK_PRIORITY, &dummy_two);
    printf("Tasks created\n");
    
    // Delay to get system locked up
    vTaskDelay(5000);

    // Get info about tasks
    vTaskGetInfo(dummy_one, &first_status, pdTRUE, eInvalid);
    vTaskGetInfo(dummy_two, &second_status, pdTRUE, eInvalid);
    eTaskState dummy_one_state = first_status.eCurrentState;
    eTaskState dummy_two_state = second_status.eCurrentState;

    // Both counters should increment twice before the deadlock, blocking both threads
    TEST_ASSERT_EQUAL(2, dummy_one_state);
    TEST_ASSERT_EQUAL(2, dummy_two_state);
    TEST_ASSERT_EQUAL(2, first.counter);
    TEST_ASSERT_EQUAL(12, second.counter);

    vTaskDelete(dummy_one);
    vTaskDelete(dummy_two);
}

// Tests orphaned_lock function
void test_orphaned_lock() {
    SemaphoreHandle_t semaphore = xSemaphoreCreateCounting(1, 1);
    int counter = 0;
    int status;
    int sema_count;

    // Send the first thread
    status = orphaned_lock(semaphore, &counter, 1000);
    sema_count = uxSemaphoreGetCount(semaphore);
    TEST_ASSERT_EQUAL(0, status);
    TEST_ASSERT_EQUAL(1, counter);
    TEST_ASSERT_EQUAL(0, sema_count);

    // Send the second thread
    status = orphaned_lock(semaphore, &counter, 1000);
    sema_count = uxSemaphoreGetCount(semaphore);
    TEST_ASSERT_EQUAL(0, status);
    TEST_ASSERT_EQUAL(1, counter);
    TEST_ASSERT_EQUAL(0, sema_count);
}

// Tests unorphaned_lock function
void test_unorphaned_lock() {
    SemaphoreHandle_t semaphore = xSemaphoreCreateCounting(1, 1);
    int counter = 0;
    int status;
    int sema_count;

    // Test first thread
    status = unorphaned_lock(semaphore, &counter, 1000);
    sema_count = uxSemaphoreGetCount(semaphore);
    TEST_ASSERT_EQUAL(1, status);
    TEST_ASSERT_EQUAL(1, counter);
    TEST_ASSERT_EQUAL(1, sema_count);

    // Test second thread
    status = unorphaned_lock(semaphore, &counter, 1000);
    sema_count = uxSemaphoreGetCount(semaphore);
    TEST_ASSERT_EQUAL(1, status);
    TEST_ASSERT_EQUAL(2, counter);
    TEST_ASSERT_EQUAL(1, sema_count);
}

//-------------TEST THREAD---------------
void test_thread(void *args) {
    printf("Starting tests\n");
    UNITY_BEGIN();
    RUN_TEST(test_blocked);
    RUN_TEST(test_unblocked);
    RUN_TEST(test_deadlock);
    RUN_TEST(test_orphaned_lock);
    RUN_TEST(test_unorphaned_lock);
    UNITY_END();
    sleep_ms(10000);
}

void dummy_thread(void *args) {
    while(1) {
        vTaskDelay(100);
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
