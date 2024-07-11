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
    gpio_set_direction(LED, GPIO_MODE_OUTPUT);
    gpio_set_direction(BT, GPIO_MODE_INPUT);

    gpio_set_pull_mode(BT, GPIO_PULLDOWN_ONLY);
}