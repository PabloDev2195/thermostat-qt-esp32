#include "adc.h"
#include "esp32_adc.h"

/**
 * @brief Initialize the ADC module.
 *
 * Initializes the underlying hardware-specific ADC implementation.
 *
 * @return
 * - ADC_STATUS_OK if the ADC was initialized successfully.
 * - ADC_STATUS_ERROR if the ADC initialization failed.
 */
adc_status_t adc_init(void)
{
    return esp32_adc_init();
}

/**
 * @brief Read the raw ADC value.
 *
 * Reads an ADC conversion value using the underlying hardware-specific
 * implementation and stores the result in the provided output variable.
 *
 * @param[out] value Pointer where the raw ADC value will be stored.
 *
 * @return
 * - ADC_STATUS_OK if the ADC value was read successfully.
 * - ADC_STATUS_INVALID_PARAM if @p value is NULL.
 * - ADC_STATUS_ERROR if the ADC read operation failed.
 */
adc_status_t adc_read(uint16_t *value)
{
    adc_status_t status = ADC_STATUS_OK;
    if (value == NULL)
    {
        status = ADC_STATUS_INVALID_PARAM;
    }
    else
    {
        status = esp32_adc_read(value);
    }
    return status;
}