#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"  
#include "ble_mgr.h"
#include "adc.h"
#include "pwm.h"
#include "fan.h"

#include "temperature.h"

static const char *TAG = "MAIN"; 

static void temperatureControl_task(void *pvParameters)
{
    float temp_c;
    for (;;) 
    {
        temperature_status_t status = temperature_read(&temp_c);

        if (status == TEMPERATURE_STATUS_OK) 
        {
            ble_manager_update_temperature(temp_c);
        } 
        else 
        {
            ESP_LOGW(TAG, "Failed to read temperature sensor");
        }

        vTaskDelay(pdMS_TO_TICKS(200));
    }
}

void app_main(void)
{
    ble_manager_init();
    adc_init();
    pwm_init();
    

    xTaskCreate(temperatureControl_task, "temperatureControl_task", 2048, NULL, 1, NULL);
    fan_task_create();
}