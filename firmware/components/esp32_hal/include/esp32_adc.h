#ifndef ESP32_ADC_H

#define ESP32_ADC_H

#include <stdint.h>

typedef enum
{
    ESP32_ADC_STATUS_OK = 0,
    ESP32_ADC_STATUS_ERROR,
    ESP32_ADC_STATUS_INVALID_PARAM

} esp32_adc_status_t;

/**
 * @brief Initialize the ESP32 ADC hardware.
 *
 * Configures the ADC unit and the selected analog input channel.
 *
 * @return
 * - ESP32_ADC_STATUS_OK if the ADC was initialized successfully.
 * - ESP32_ADC_STATUS_ERROR if the ADC initialization or channel configuration failed.
 */
esp32_adc_status_t esp32_adc_init(void);

/**
 * @brief Read the raw ADC value from the configured ESP32 ADC channel.
 *
 * Performs a single ADC conversion and stores the resulting raw
 * value in the provided output variable.
 *
 * @param[out] value Pointer where the raw ADC value will be stored.
 *
 * @return
 * - ESP32_ADC_STATUS_OK if the ADC conversion was successful.
 * - ESP32_ADC_STATUS_INVALID_PARAM if @p value is NULL.
 * - ESP32_ADC_STATUS_ERROR if the ADC read operation failed.
 */
esp32_adc_status_t esp32_adc_read(uint16_t *value);

#endif /* ESP32_ADC_H */