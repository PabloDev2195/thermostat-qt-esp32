#include "temperature.h"
#include "adc.h"

#define TEMPERATURE_SENSOR_SCALING_FACTOR 100.0f
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