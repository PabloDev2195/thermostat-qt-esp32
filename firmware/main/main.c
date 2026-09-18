#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"  
#include "ble_mgr.h"
#include "adc.h"
#include "fan.h"

#include "temperature.h"

static const char *TAG = "MAIN"; 

static void motorControl_task(void *pvParameters)
{
    for (;;) 
    {
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

static void heaterControl_task(void *pvParameters)
{
    for (;;) 
    {
        vTaskDelay(pdMS_TO_TICKS(200));
    }
}

static void compressorControl_task(void *pvParameters)
{
    for (;;) 
    {
        ESP_LOGI(TAG, "Fan LOW");
        fan_set_level(FAN_LEVEL_LOW);
        vTaskDelay(pdMS_TO_TICKS(3000));

        ESP_LOGI(TAG, "Fan MEDIUM");
        fan_set_level(FAN_LEVEL_MEDIUM);
        vTaskDelay(pdMS_TO_TICKS(3000));

        ESP_LOGI(TAG, "Fan HIGH");
        fan_set_level(FAN_LEVEL_HIGH);
        vTaskDelay(pdMS_TO_TICKS(3000));
    }
}

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
    if (fan_init() != FAN_OK)
    {
        ESP_LOGE(TAG, "Fan initialization failed");
        return;
    }

    xTaskCreate(temperatureControl_task, "temperatureControl_task", 2048, NULL, 1, NULL);
    xTaskCreate(compressorControl_task, "compressorControl_task", 2048, NULL, 2, NULL);
    xTaskCreate(motorControl_task, "motorControl_task", 2048, NULL, 3, NULL);
    xTaskCreate(heaterControl_task, "heaterControl_task", 2048, NULL, 5, NULL);
}