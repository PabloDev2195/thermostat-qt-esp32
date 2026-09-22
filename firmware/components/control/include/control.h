/**
 * @file control.h
 * @brief Thermostat control logic interface.
 */

#ifndef CONTROL_H
#define CONTROL_H

#include <stdbool.h>

/**
 * @brief Thermostat operating mode.
 */
typedef enum
{
    CONTROL_MODE_OFF = 0,
    CONTROL_MODE_AUTO
} control_mode_t;

/**
 * @brief Current thermostat state.
 */
typedef enum
{
    CONTROL_STATE_OFF = 0,
    CONTROL_STATE_HEAT,
    CONTROL_STATE_COOL
} control_state_t;

/**
 * @brief Update thermostat control.
 *
 * @param current_temperature Current temperature in degrees Celsius.
 * @param setpoint Desired temperature in degrees Celsius.
 *
 * @return 0 on success, negative value on error.
 */
int control_update(float current_temperature,float setpoint);

/**
 * @brief Get current thermostat state.
 *
 * @return Current control state.
 */
control_state_t control_get_state(void);

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

#endif /* CONTROL_H */