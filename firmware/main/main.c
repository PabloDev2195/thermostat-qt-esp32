#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"  

#include "ble_mgr.h"
#include "adc.h"
#include "pwm.h"
#include "gpio.h"

#include "fan.h"
#include "control.h"
#include "temperature.h"


void app_main(void)
{
    ble_manager_init();
    adc_init();
    pwm_init();
    gpio_init();

    temperature_task_create();
    fan_task_create();
    control_task_create();
}