#include <stdio.h>
#include <FreeRTOS.h>
#include <semphr.h>
#include <task.h>
#include <pico/stdlib.h>
#include <pico/multicore.h>
#include <pico/cyw43_arch.h>
#include "thread_helpers.h"

int main(void) {
    stdio_init_all();
    hard_assert(cyw43_arch_init() == PICO_OK);
    TaskHandle_t master;
    sleep_ms(10000);
    xTaskCreate(master_thread, "MasterThread",
                MAIN_TASK_STACK_SIZE, NULL, MAIN_TASK_PRIORITY, &master);

    // xTaskCreate(main_thread, "MainThread",
    //             SIDE_TASK_STACK_SIZE, NULL, SIDE_TASK_PRIORITY, &main);
    // xTaskCreate(side_thread, "SideThread",
    //             SIDE_TASK_STACK_SIZE, NULL, SIDE_TASK_PRIORITY, &side);
    vTaskStartScheduler();
	return 0;
}