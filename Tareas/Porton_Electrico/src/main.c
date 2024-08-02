#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "sdkconfig.h"

// Definición de pines
#define SWITCH_OPEN_PIN      12
#define SWITCH_CLOSE_PIN     13
#define SENSOR_OBST_PIN      14
#define MOTOR_PIN            15
#define EMERGENCY_STOP_PIN   16

// Definición de estados del motor
#define MOTOR_STOP           0
#define MOTOR_OPEN           1
#define MOTOR_CLOSE          2

// Definición de estados del portón
typedef enum {
    GATE_IDLE,
    GATE_OPENING,
    GATE_CLOSING,
    GATE_STOPPED,
    GATE_EMERGENCY_STOP
} gate_state_t;

// Definición de tiempos
#define OPEN_CLOSE_TIMEOUT_MS   10000  // Tiempo máximo para abrir/cerrar el portón

static const char *TAG = "GATE_CONTROL";
static TimerHandle_t xTimer = NULL;

void motor_control(int command) {
    switch (command) {
        case MOTOR_OPEN:
            ESP_LOGI(TAG, "Motor: OPEN");
            gpio_set_level(MOTOR_PIN, 1); // Encender el motor para abrir
            break;
        case MOTOR_CLOSE:
            ESP_LOGI(TAG, "Motor: CLOSE");
            gpio_set_level(MOTOR_PIN, 0); // Encender el motor para cerrar
            break;
        case MOTOR_STOP:
        default:
            ESP_LOGI(TAG, "Motor: STOP");
            gpio_set_level(MOTOR_PIN, 0); // Detener el motor
            break;
    }
}

void initialize_gpio() {
    gpio_config_t io_conf;
    
    // Configuración de los pines de los switches, sensor y botón de emergencia
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = (1ULL << SWITCH_OPEN_PIN) | (1ULL << SWITCH_CLOSE_PIN) | (1ULL << SENSOR_OBST_PIN) | (1ULL << EMERGENCY_STOP_PIN);
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 1;
    gpio_config(&io_conf);
    
    // Configuración del pin del motor
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = (1ULL << MOTOR_PIN);
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf);
}

void timer_callback(TimerHandle_t xTimer) {
    ESP_LOGI(TAG, "Operation timeout, stopping gate.");
    motor_control(MOTOR_STOP);
}

void gate_control_task(void *pvParameter) {
    gate_state_t gate_state = GATE_IDLE;
    
    while (1) {
        int open_switch_state = gpio_get_level(SWITCH_OPEN_PIN);
        int close_switch_state = gpio_get_level(SWITCH_CLOSE_PIN);
        int obstacle_sensor_state = gpio_get_level(SENSOR_OBST_PIN);
        int emergency_stop_state = gpio_get_level(EMERGENCY_STOP_PIN);

        switch (gate_state) {
            case GATE_IDLE:
                if (emergency_stop_state == 1) {
                    gate_state = GATE_EMERGENCY_STOP;
                } else if (obstacle_sensor_state == 1) {
                    gate_state = GATE_STOPPED;
                } else if (open_switch_state == 1) {
                    ESP_LOGI(TAG, "Starting to open gate.");
                    gate_state = GATE_OPENING;
                    xTimerStart(xTimer, 0);
                } else if (close_switch_state == 1) {
                    ESP_LOGI(TAG, "Starting to close gate.");
                    gate_state = GATE_CLOSING;
                    xTimerStart(xTimer, 0);
                }
                break;
                
            case GATE_OPENING:
                if (emergency_stop_state == 1 || obstacle_sensor_state == 1) {
                    ESP_LOGI(TAG, "Obstacle detected or emergency stop during opening.");
                    gate_state = GATE_STOPPED;
                } else if (open_switch_state == 0) {
                    ESP_LOGI(TAG, "Gate opened successfully.");
                    gate_state = GATE_IDLE;
                    xTimerStop(xTimer, 0);
                } else {
                    motor_control(MOTOR_OPEN);
                }
                break;

            case GATE_CLOSING:
                if (emergency_stop_state == 1 || obstacle_sensor_state == 1) {
                    ESP_LOGI(TAG, "Obstacle detected or emergency stop during closing.");
                    gate_state = GATE_STOPPED;
                } else if (close_switch_state == 0) {
                    ESP_LOGI(TAG, "Gate closed successfully.");
                    gate_state = GATE_IDLE;
                    xTimerStop(xTimer, 0);
                } else {
                    motor_control(MOTOR_CLOSE);
                }
                break;

            case GATE_STOPPED:
                motor_control(MOTOR_STOP);
                if (emergency_stop_state == 0 && obstacle_sensor_state == 0) {
                    gate_state = GATE_IDLE;
                }
                break;
                
            case GATE_EMERGENCY_STOP:
                motor_control(MOTOR_STOP);
                ESP_LOGI(TAG, "Emergency stop activated!");
                if (emergency_stop_state == 0) {
                    gate_state = GATE_IDLE;
                }
                break;
                
            default:
                gate_state = GATE_IDLE;
                break;
        }
        
        ESP_LOGI(TAG, "Gate state: %d", gate_state);
        vTaskDelay(100 / portTICK_PERIOD_MS); // Esperar 100ms antes de leer los pines nuevamente
    }
}

void app_main(void) {
    ESP_LOGI(TAG, "Initializing GPIO...");
    initialize_gpio();
    ESP_LOGI(TAG, "Creating timer...");
    xTimer = xTimerCreate("GateTimer", pdMS_TO_TICKS(OPEN_CLOSE_TIMEOUT_MS), pdFALSE, (void *)0, timer_callback);
    if (xTimer == NULL) {
        ESP_LOGE(TAG, "Timer creation failed!");
        return;
    }
    ESP_LOGI(TAG, "Starting gate control task...");
    xTaskCreate(&gate_control_task, "gate_control_task", 2048, NULL, 5, NULL);
}
