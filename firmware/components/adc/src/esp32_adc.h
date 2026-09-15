#ifndef ESP32_ADC_H

#define ESP32_ADC_H

#include <stdint.h>

#include "adc.h"

/**
 * @brief Initialize the ESP32 ADC hardware.
 *
 * Configures the ADC unit and the selected analog input channel.
 *
 * @return
 * - ADC_STATUS_OK if the ADC was initialized successfully.
 * - ADC_STATUS_ERROR if the ADC initialization or channel configuration failed.
 */
adc_status_t esp32_adc_init(void);

/**
 * @brief Read the raw ADC value from the configured ESP32 ADC channel.
 *
 * Performs a single ADC conversion and stores the resulting raw
 * value in the provided output variable.
 *
 * @param[out] value Pointer where the raw ADC value will be stored.
 *
 * @return
 * - ADC_STATUS_OK if the ADC conversion was successful.
 * - ADC_STATUS_INVALID_PARAM if @p value is NULL.
 * - ADC_STATUS_ERROR if the ADC read operation failed.
 */
adc_status_t esp32_adc_read(uint16_t *value);

#endif /* ESP32_ADC_H */