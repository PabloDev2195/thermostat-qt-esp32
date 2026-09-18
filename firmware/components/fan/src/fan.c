/**
 * @file fan.c
 * @brief Generic fan control implementation using PWM.
 *
 * Maps abstract fan operating levels to PWM duty-cycle values.
 */

#include "fan.h"

#include "pwm.h"

/**
 * @brief PWM duty cycle for the fan OFF state.
 */
#define FAN_DUTY_OFF       0

/**
 * @brief PWM duty cycle for the fan LOW state.
 *
 * Corresponds to approximately 20% duty cycle.
 */
#define FAN_DUTY_LOW       (20 * 1023 / 100)

/**
 * @brief PWM duty cycle for the fan MEDIUM state.
 *
 * Corresponds to approximately 55% duty cycle.
 */
#define FAN_DUTY_MEDIUM    (55 * 1023 / 100)

/**
 * @brief PWM duty cycle for the fan HIGH state.
 *
 * Corresponds to 100% duty cycle.
 */
#define FAN_DUTY_HIGH      (100 * 1023 / 100)

/**
 * @brief Current fan operating level.
 *
 * The fan starts in the OFF state.
 */
static fan_level_t current_level = FAN_LEVEL_OFF;

/**
 * @brief Initializes the fan module.
 *
 * Initializes the underlying PWM module and sets the fan
 * to the OFF state.
 *
 * @return
 * - FAN_OK if the fan was initialized successfully.
 * - FAN_ERROR if PWM initialization or configuration fails.
 */
fan_status_t fan_init(void)
{
    fan_status_t status = FAN_OK;

    if (pwm_init() != PWM_OK)
    {
        status = FAN_ERROR;
    }
    else if (pwm_set_duty(FAN_DUTY_OFF) != PWM_OK)
    {
        status = FAN_ERROR;
    }
    else
    {
        current_level = FAN_LEVEL_OFF;
    }

    return status;
}

/**
 * @brief Sets the fan operating level.
 *
 * Converts the requested fan level into the corresponding
 * PWM duty cycle and applies it through the generic PWM interface.
 *
 * @param[in] level Desired fan operating level.
 *
 * @return
 * - FAN_OK if the fan level was changed successfully.
 * - FAN_INVALID_LEVEL if the specified level is invalid.
 * - FAN_ERROR if the PWM operation fails.
 */
fan_status_t fan_set_level(fan_level_t level)
{
    uint16_t duty = FAN_DUTY_OFF;
    fan_status_t status = FAN_OK;

    switch (level)
    {
        case FAN_LEVEL_OFF:
            duty = FAN_DUTY_OFF;
            break;

        case FAN_LEVEL_LOW:
            duty = FAN_DUTY_LOW;
            break;

        case FAN_LEVEL_MEDIUM:
            duty = FAN_DUTY_MEDIUM;
            break;

        case FAN_LEVEL_HIGH:
            duty = FAN_DUTY_HIGH;
            break;

        default:
            return FAN_INVALID_LEVEL;
    }

    if (pwm_set_duty(duty) != PWM_OK)
    {
        status = FAN_ERROR;
    }
    else
    {
        current_level = level;
    }

    return status;
}

/**
 * @brief Gets the current fan operating level.
 *
 * @return Current fan operating level.
 */
fan_level_t fan_get_level(void)
{
    return current_level;
}