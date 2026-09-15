#ifndef ADC_H

#define ADC_H

#include <stdint.h>

/**
 * @brief ADC operation status codes.
 */
typedef enum
{
    /** Operation completed successfully. */
    ADC_STATUS_OK = 0,

    /** Operation failed. */
    ADC_STATUS_ERROR,

    /** One or more function parameters are invalid. */
    ADC_STATUS_INVALID_PARAM

} adc_status_t;

/**
 * @brief Initialize the ADC module.
 *
 * Initializes the ADC hardware through the underlying
 * hardware-specific implementation.
 *
 * @return
 * - ADC_STATUS_OK if the ADC was initialized successfully.
 * - ADC_STATUS_ERROR if the initialization failed.
 */
adc_status_t adc_init(void);

/**
 * @brief Read the raw ADC value.
 *
 * Performs an ADC conversion and stores the raw result
 * in the provided output variable.
 *
 * @param[out] value Pointer where the raw ADC value will be stored.
 *
 * @return
 * - ADC_STATUS_OK if the ADC value was read successfully.
 * - ADC_STATUS_INVALID_PARAM if @p value is NULL.
 * - ADC_STATUS_ERROR if the ADC read operation failed.
 */
adc_status_t adc_read(uint16_t *value);

#endif /* ADC_H */