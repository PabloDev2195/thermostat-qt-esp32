/**
 * @file pwm.c
 * @brief Generic PWM implementation using the ESP32 PWM driver.
 *
 * This module provides a hardware-independent PWM interface and
 * delegates the actual hardware operations to the ESP32-specific
 * implementation.
 */

#include "pwm.h"

#include "esp32_pwm.h"

/**
 * @brief Maximum supported PWM duty-cycle value.
 *
 * The current ESP32 PWM hardware is configured with a 10-bit
 * resolution, resulting in a valid duty-cycle range from 0 to 100.
 */
#define MAX_DUTY_CYCLE    100

/**
 * @brief Initializes the PWM module.
 *
 * Initializes the underlying ESP32 PWM hardware.
 *
 * @return
 * - PWM_OK if the PWM module was initialized successfully.
 * - PWM_ERROR if the underlying PWM hardware could not be initialized.
 */
pwm_status_t pwm_init(void)
{
    pwm_status_t status = PWM_OK;
    if (esp32_pwm_init() != ESP_OK)
    {
        status = PWM_ERROR;
    }

    return status;
}

/**
 * @brief Sets the PWM duty cycle.
 *
 * Validates the requested duty cycle and forwards it to the
 * platform-specific PWM implementation.
 *
 * @param[in] uDuty Duty-cycle value.
 *                  Valid values range from 0 to 100.
 *
 * @return
 * - PWM_OK if the duty cycle was updated successfully.
 * - PWM_INVALID_DUTY if uDuty is outside the valid range.
 * - PWM_ERROR if the underlying PWM hardware reports an error.
 */
pwm_status_t pwm_set_duty(uint8_t uDuty)
{
    pwm_status_t status = PWM_OK;
    if (uDuty > MAX_DUTY_CYCLE)
    {
        status = PWM_INVALID_DUTY;
    }
    else
    {
        if (esp32_pwm_set_duty(uDuty) != ESP_OK)
        {
            status = PWM_ERROR;
        }
    }
    return status;
}