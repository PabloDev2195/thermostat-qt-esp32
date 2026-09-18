 /**
  * @file pwm.h
  * @brief Generic PWM interface.
  *
  * Provides a hardware-independent interface for PWM initialization
  * and duty-cycle control.
  */

#ifndef PWM_H
#define PWM_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief PWM operation status.
 */
typedef enum
{
    /**
     * @brief Operation completed successfully.
     */
    PWM_OK = 0,

    /**
     * @brief Generic PWM error.
     */
    PWM_ERROR,

    /**
     * @brief Invalid duty-cycle value.
     */
    PWM_INVALID_DUTY

} pwm_status_t;

/**
 * @brief Initializes the PWM module.
 *
 * Initializes the underlying PWM hardware through the platform-specific
 * implementation.
 *
 * @return
 * - PWM_OK if the PWM module was initialized successfully.
 * - PWM_ERROR if the underlying PWM hardware could not be initialized.
 */
pwm_status_t pwm_init(void);

/**
 * @brief Sets the PWM duty cycle.
 *
 * Updates the duty cycle through the generic PWM interface.
 *
 * @param[in] uDuty Duty-cycle value.
 *                  With the current 10-bit PWM configuration,
 *                  valid values range from 0 to 100.
 *
 * @return
 * - PWM_OK if the duty cycle was updated successfully.
 * - PWM_INVALID_DUTY if uDuty is outside the valid range.
 * - PWM_ERROR if the underlying PWM hardware reports an error.
 */
pwm_status_t pwm_set_duty(uint8_t uDuty);

#ifdef __cplusplus
}
#endif

#endif /* PWM_H */