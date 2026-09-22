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
 * @brief Creates the temperature task.
 *
 * The task reads the temperature sensor and updates the BLE manager.
 */
void temperature_task_create(void);

/**
 * @brief Gets the current measured temperature.
 *
 * Returns the latest temperature value measured by the temperature
 * module.
 *
 * @return Current temperature in degrees Celsius.
 */
float temperature_get_currentTemperature(void);

#endif /* TEMPERATURE_H */