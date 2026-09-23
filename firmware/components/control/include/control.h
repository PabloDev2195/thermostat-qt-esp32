/**
 * @file control.h
 * @brief Thermostat control logic interface.
 */

#ifndef CONTROL_H
#define CONTROL_H

#include <stdbool.h>
#include <stdint.h>

/**
 * @brief Thermostat operating mode.
 */
typedef enum
{
    CONTROL_MODE_OFF = 0,
    CONTROL_MODE_AUTO,
    CONTROL_MODE_ECO
} control_mode_t;

/**
 * @brief Current thermostat state.
 */
typedef enum
{
    CONTROL_STATE_IDLE = 0,
    CONTROL_STATE_HEAT,
    CONTROL_STATE_COOL,
} control_state_t;

/**
 * @brief Update thermostat control.
 *
 * @param current_temperature Current temperature in degrees Celsius.
 * @param setpoint Desired temperature in degrees Celsius.
 *
 * @return 0 on success, negative value on error.
 */
uint8_t control_update(float current_temperature,float setpoint);

/**
 * @brief Get current thermostat state.
 *
 * @return Current control state.
 */
control_state_t control_get_state(void);

/**
 * @brief Gets the current operating mode of the control module.
 *
 * Retrieves the operating mode currently configured in the control module.
 *
 * @return Current operating mode (control_mode_t).
 */
control_mode_t control_get_mode(void);

/**
 * @brief Creates the thermostat control task and command queue.
 *
 * Initializes the FreeRTOS resources required by the control module,
 * including the control command queue and the periodic control task.
 *
 * The control task periodically reads the current temperature and
 * evaluates the thermostat control logic.
 */
void control_task_create(void);

/**
 * @brief Sets the desired temperature setpoint.
 *
 * Updates the temperature setpoint used by the control module.
 * The input value is represented in tenths of a degree Celsius.
 *
 * For example:
 * - 220 -> 22.0 °C
 * - 255 -> 25.5 °C
 * - 300 -> 30.0 °C
 *
 * @param setpoint Temperature setpoint in tenths of a degree Celsius.
 *
 * @note The received value is converted internally to a floating-point
 *       temperature value by dividing it by 10.0.
 */
void control_set_setpoint(uint16_t setpoint);

/**
 * @brief Sets the operating mode of the control module.
 *
 * Configures the control module operating mode.
 *
 * @param[in] mode Operating mode value corresponding to control_mode_t.
 *
 * @note The input value must match a valid control_mode_t enumerator.
 */
void control_set_mode(uint8_t mode);

#endif /* CONTROL_H */