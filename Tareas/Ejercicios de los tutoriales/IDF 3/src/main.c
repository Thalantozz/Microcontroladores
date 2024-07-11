#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#define LED 2
#define BT 17

void init_hw(void);

void app_main() {

    init_hw();

    while(1){

        int stable_bt = gpio_get_level(BT);
        if(stable_bt){

            gpio_set_level(LED, 1);
        }
        else{

            gpio_set_level(LED, 0);
        }
        vTaskDelay(200 / portTICK_PERIOD_MS);
    }
}

void init_hw(void){
    gpio_config_t io_config;

    io_config.mode = GPIO_MODE_INPUT;
    io_config.pin_bit_mask = (1 << BT);
    io_config.pull_down_en = GPIO_PULLDOWN_ONLY;
    io_config.pull_up_en = GPIO_PULLUP_DISABLE;
    io_config.intr_type = GPIO_INTR_DISABLE;
    gpio_config(&io_config);

    io_config.mode = GPIO_MODE_INPUT;
    io_config.pin_bit_mask = (1 << LED);
    io_config.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_config.pull_up_en = GPIO_PULLUP_DISABLE;
    io_config.intr_type = GPIO_INTR_DISABLE;
    gpio_config(&io_config);
}