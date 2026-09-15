#ifndef TEMPERATURE_H

#define TEMPERATURE_H

#include <stdint.h>

/**
 * @brief Temperature operation status codes.
 */
typedef enum
{
    /** Operation completed successfully. */
    TEMPERATURE_STATUS_OK = 0,

    /** Operation failed. */
    TEMPERATURE_STATUS_ERROR

} temperature_status_t;

/**
 * @brief Read the current temperature.
 *
 * Reads the raw ADC value through the ADC abstraction layer
 * and converts it to a temperature value.
 *
 * @param[out] temperature Pointer where the temperature value
 *                         will be stored, in degrees Celsius.
 *
 * @return
 * - TEMPERATURE_STATUS_OK if the temperature was read successfully.
 * - TEMPERATURE_STATUS_ERROR if the temperature pointer is NULL
 *   or the ADC read operation fails.
 */
temperature_status_t temperature_read(float *temperature);

#endif /* TEMPERATURE_H */