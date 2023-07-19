#include "FreeRTOSConfig.h"
#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"
#include <stack>
#include <stdio.h>
#include <string>
#include <memory>

#include "keypad.h"
#include "pico/time.h"
#include "portmacro.h"
#include "screen.h"
#include "u8g2.h"
#include "u8g2_rp2040_hal.h"
#include "screens/calculator.h"
#include "FreeRTOS.h"
#include "task.h"
#include "maths.h"

update_fn curr_upd_fn;


void vApplicationMallocFailedHook() {
    printf("SAD\n");
    configASSERT(0);
    for(;;);
}

void vApplicationTickHook() {

}

void vApplicationStackOverflowHook(TaskHandle_t pt, char *taskname) {
    printf("stack overflow @ %s\n", taskname);
    configASSERT(0);
    for(;;);
}

void dot(u8g2_t *u8g2) {
}


static void task1(void* u8) {
    TickType_t nextWakeTime = xTaskGetTickCount();
    u8g2_t *u8g2 = (u8g2_t *)u8;
    int n = 0;
    curr_upd_fn = screens::calculator::update;

    while (true) {
        curr_upd_fn(u8g2);
        vTaskDelayUntil(&nextWakeTime, 50);
    }
}
    u8g2_t u8g2;

int main() {
    cyw43_arch_init();
    stdio_init_all();
    //sleep_ms(5000);
    u8g2_Setup_st7565_erc12864_alt_f(&u8g2, U8G2_R0, u8x8_byte_4wire_sw_spi, display_rp2040::cb_gpio_delay);
    u8g2_InitDisplay(&u8g2);
    u8g2_SetPowerSave(&u8g2, false);
    u8g2_SetContrast(&u8g2, 70);

    keypad::keypad main_kp(5, 4, keypad::DEFAULT_MAIN_KP_ROWS, keypad::DEFAULT_MAIN_KP_COLS, keypad::DEFAULT_MAIN_KP_KEYS);

    // u8g2_ClearDisplay(&u8g2);
    printf("All setup, go\n");

    xTaskCreate(task1, "ui", 16384, &u8g2, 0, 0);

    printf("Still OK\n");

    vTaskStartScheduler();
    printf("Continuing\n");
    for (;;);
}