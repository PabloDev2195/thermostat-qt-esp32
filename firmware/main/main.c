#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "ble_mgr.h"

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
        vTaskDelay(pdMS_TO_TICKS(200));
    }
}

static void heartbeat_task(void *pvParameters)
{
    for (;;) 
    {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void app_main(void)
{
    ble_manager_init();

    xTaskCreate(heartbeat_task, "heartbeat_task", 2048, NULL, 1, NULL);
    xTaskCreate(compressorControl_task, "compressorControl_task", 2048, NULL, 2, NULL);
    xTaskCreate(motorControl_task, "motorControl_task", 2048, NULL, 3, NULL);
    xTaskCreate(heaterControl_task, "heaterControl_task", 2048, NULL, 5, NULL);
}