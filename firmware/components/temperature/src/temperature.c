#include "temperature.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "adc.h"
#include "ble_mgr.h"

#define TEMPERATURE_TASK_STACK_SIZE    2048
#define TEMPERATURE_TASK_PRIORITY      2
#define TEMPERATURE_QUEUE_LENGTH       1

#define TEMPERATURE_SENSOR_SCALING_FACTOR 100.0f

temperature_status_t temperature_read(float *temperature);

static float current_temperature = 0.0f;


/**
 * @brief FreeRTOS task for temperature monitoring.
 *
 * Periodically reads the temperature sensor and updates
 * the current temperature value.
 *
 * @param pvParameters Task parameters (unused).
 */
static void temperature_task(void *pvParameters)
{
    float temp_c;
    for (;;) 
    {
        temperature_status_t status = temperature_read(&temp_c);

        if (status == TEMPERATURE_STATUS_OK) 
        {
            current_temperature = temp_c;
            ble_manager_update_temperature(temp_c);
        } 
        else 
        {

        }
        vTaskDelay(pdMS_TO_TICKS(200));
    }
}

/**
 * @brief Creates the temperature task.
 *
 * The task reads the temperature sensor and updates the BLE manager.
 */
void temperature_task_create(void)
{
    xTaskCreate(
        temperature_task,
        "temperature_task",
        TEMPERATURE_TASK_STACK_SIZE,
        NULL,
        TEMPERATURE_TASK_PRIORITY,
        NULL
    );
}

/**
 * @brief Read the current temperature.
 *
 * Reads the raw ADC value through the ADC abstraction layer and
 * converts it to a simulated temperature value.
 *
 * The conversion is performed by dividing the raw ADC value by
 * 100. This conversion is intended for simulation purposes and
 * does not represent a real temperature sensor characteristic.
 *
 * @param[out] temperature Pointer where the calculated temperature
 *                         will be stored, in degrees Celsius.
 *
 * @return
 * - TEMPERATURE_STATUS_OK if the temperature was read successfully.
 * - TEMPERATURE_STATUS_ERROR if the temperature pointer is NULL
 *   or the ADC read operation fails.
 */
temperature_status_t temperature_read(float *temperature)
{
    uint16_t adc_value;
    temperature_status_t status = TEMPERATURE_STATUS_OK;

    if (temperature == NULL)
    {
        status = TEMPERATURE_STATUS_ERROR;
    }
    else
    {
        if (adc_read(&adc_value) != ADC_STATUS_OK)
        {
            status = TEMPERATURE_STATUS_ERROR;
        }
        else
        {
            *temperature = (float)(adc_value / TEMPERATURE_SENSOR_SCALING_FACTOR);
        }
    }
    return status;
}

/**
 * @brief Gets the current measured temperature.
 *
 * Returns the latest temperature value obtained from the temperature
 * measurement module.
 *
 * @return Current temperature in degrees Celsius.
 */
float temperature_get_currentTemperature(void)
{
    return current_temperature;
}