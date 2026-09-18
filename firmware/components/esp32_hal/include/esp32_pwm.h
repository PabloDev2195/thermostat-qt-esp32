/**
 * @file esp32_pwm.h
 * @brief ESP32-specific PWM interface.
 *
 * This module provides a hardware-specific abstraction for generating
 * PWM signals using the ESP32 LEDC peripheral.
 */

#ifndef ESP32_PWM_H
#define ESP32_PWM_H

#include <stdint.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief GPIO used as PWM output.
 *
 * The PWM signal is generated on GPIO13.
 */
#define PWM_GPIO            13

/**
 * @brief PWM operating frequency in Hz.
 *
 * The PWM signal operates at 25 kHz.
 */
#define PWM_FREQUENCY_HZ    25000

/**
 * @brief PWM resolution in bits.
 *
 * A 10-bit resolution provides duty-cycle values from 0 to 1023.
 */
#define PWM_RESOLUTION      10

/**
 * @brief Initializes the ESP32 PWM peripheral.
 *
 * Configures the LEDC timer and PWM channel using the configured
 * GPIO, frequency, and resolution.
 *
 * @return
 * - ESP_OK if the PWM peripheral was initialized successfully.
 * - An appropriate ESP-IDF error code if initialization fails.
 */
esp_err_t esp32_pwm_init(void);

/**
 * @brief Sets the PWM duty cycle.
 *
 * Updates the duty cycle of the configured PWM channel.
 *
 * @param[in] uDuty Duty-cycle value.
 *                  For 10-bit resolution, valid values are 0 to 1023.
 *
 * @return
 * - ESP_OK if the duty cycle was updated successfully.
 * - ESP_ERR_INVALID_ARG if the duty cycle is outside the valid range.
 * - An appropriate ESP-IDF error code if the PWM update fails.
 */
esp_err_t esp32_pwm_set_duty(uint8_t uDuty);

#ifdef __cplusplus
}
#endif

#endif /* ESP32_PWM_H */