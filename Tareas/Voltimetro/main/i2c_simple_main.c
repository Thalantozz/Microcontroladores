

#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>

#include <freertos/FreeRTOS.h>
#include <freertos/timers.h>
#include <freertos/task.h>
#include <driver/adc.h>
#include <driver/gpio.h>
#include "esp_log.h"
#include "driver/i2c.h"
#include "esp_adc/adc_oneshot.h"
#include "i2c-lcd.h"
#include "esp_task_wdt.h"

#define TOTAL 100
const float adc_valor = 0.0046875; /

void init_priferic(void);
float CALCULOS(char mues[]);
void IMPRESION_LCD(float *resul);

static const char *TAG = "i2c-simple-example";

adc1_channel_t adc_pot = ADC1_CHANNEL_6; 


/**
 * @brief 
 */
static esp_err_t i2c_master_init(void)
{
    int i2c_master_port = I2C_NUM_0;

    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = GPIO_NUM_21,
        .scl_io_num = GPIO_NUM_22,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = 100000,
    };

    i2c_param_config(i2c_master_port, &conf);

    return i2c_driver_install(i2c_master_port, conf.mode, 0, 0, 0);
}


void app_main(void)
{

	float muestreo[TOTAL] = {};
	float resultado = 0, resul_anterior = 0;
	int cont = 0;


    ESP_ERROR_CHECK(i2c_master_init());
    ESP_LOGI(TAG, "I2C initialized successfully");

    init_priferic();

    lcd_init();
    lcd_put_cur(0, 0);

    lcd_send_string("Hello World");
    lcd_put_cur(1, 0);
    lcd_send_string("Albert Cabra");
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    lcd_clear();
    lcd_put_cur(0, 2);
    lcd_send_string("VOLTIMETRO");
    lcd_clear();


    while(1){


    	lcd_put_cur(1, 0);
    	lcd_send_string("RPM: ");


    
    	muestreo[cont] = (adc1_get_raw(adc_pot) * adc_valor); 
    	        cont++;
    	        if (cont == TOTAL) 
    	        {
    	        	cont = 0;
    	        	resul_anterior = resultado;
    	        	resultado = CALCULOS(muestreo);

    	            if (resultado != resul_anterior && resultado > 0)              
    	            {

    	            	IMPRESION_LCD(&resultado);

    	            }
    	            else{

    	            	if(resultado <= 0){

    	            		printf("No se obtuvo el valor");

    	            	}
    	            	else{

    	            		printf("Es el mismo valor anterior");
    	            	}
    	            }
    	            ESP_ERROR_CHECK_WITHOUT_ABORT(esp_task_wdt_reset());
    	        }

    	 vTaskDelay(1 / portTICK_PERIOD_MS);


    }


}

void init_priferic(void){ 


    adc1_config_width(ADC_WIDTH_BIT_12);
    ESP_ERROR_CHECK(adc1_config_channel_atten(adc_pot, ADC_ATTEN_DB_11));
}

float CALCULOS(char mues[]){


	 float sum = 0, resul = 0;

	for (int i = 0; i < TOTAL; i++)
	{
		mues[i] *= mues[i];
		sum += mues[i];
	}
	resul = sqrt(sum / 100);
	resul = round(resul * 10) / 10;   

	return resul;
}

void IMPRESION_LCD(float *resul){

	char salida[20] = {};

	lcd_put_cur(1, 3);
	sprintf(salida, "    %.1f V", *resul);
	lcd_send_string(salida);
	printf("\nResultado colocado en la LCD");

}
