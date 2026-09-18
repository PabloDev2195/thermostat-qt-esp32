/**
 * @file esp32_pwm.c
 * @brief ESP32-specific PWM implementation using the LEDC peripheral.
 */

#include "esp32_pwm.h"

#include "driver/ledc.h"

/**
 * @brief LEDC timer used for PWM generation.
 */
#define PWM_TIMER       LEDC_TIMER_0

/**
 * @brief LEDC channel used for PWM generation.
 */
#define PWM_CHANNEL     LEDC_CHANNEL_0

/**
 * @brief LEDC speed mode used by the PWM channel.
 */
#define PWM_SPEED_MODE  LEDC_LOW_SPEED_MODE

/**
 * @brief Maximum duty-cycle value supported by the configured resolution.
 *
 * For a 10-bit resolution, the valid duty-cycle range is 0 to 1023.
 */
#define PWM_MAX_DUTY    ((1 << PWM_RESOLUTION) - 1)

/**
 * @brief Initializes the ESP32 PWM peripheral.
 *
 * Configures the LEDC timer and channel using the parameters defined
 * in esp32_pwm.h.
 *
 * The PWM signal is generated on the configured GPIO using the selected
 * frequency, resolution, timer, channel, and speed mode.
 *
 * @return
 * - ESP_OK if the timer and channel were configured successfully.
 * - An appropriate ESP-IDF error code if the configuration fails.
 */
esp_err_t esp32_pwm_init(void)
{
    esp_err_t ret;

    const ledc_timer_config_t timer_config =
    {
        .speed_mode       = PWM_SPEED_MODE,
        .timer_num        = PWM_TIMER,
        .duty_resolution  = PWM_RESOLUTION,
        .freq_hz          = PWM_FREQUENCY_HZ,
        .clk_cfg          = LEDC_AUTO_CLK,
    };

    ret = ledc_timer_config(&timer_config);

    if (ret != ESP_OK)
    {
        /* Timer configuration failed. */
    }
    else
    {
        const ledc_channel_config_t channel_config =
        {
            .gpio_num   = PWM_GPIO,
            .speed_mode = PWM_SPEED_MODE,
            .channel    = PWM_CHANNEL,
            .intr_type  = LEDC_INTR_DISABLE,
            .timer_sel  = PWM_TIMER,
            .duty       = 0,
            .hpoint     = 0,
        };

        ret = ledc_channel_config(&channel_config);
    }

    return ret;
}

/**
 * @brief Sets the PWM duty cycle.
 *
 * Updates the duty cycle of the configured LEDC channel.
 *
 * @param[in] uDuty Duty-cycle value. The valid range is from 0 to
 *                  PWM_MAX_DUTY.
 *
 * @return
 * - ESP_OK if the duty cycle was updated successfully.
 * - ESP_ERR_INVALID_ARG if uDuty exceeds the maximum duty-cycle value.
 * - An appropriate ESP-IDF error code if the LEDC operation fails.
 */
esp_err_t esp32_pwm_set_duty(uint32_t uDuty)
{
    esp_err_t ret;

    if (uDuty > PWM_MAX_DUTY)
    {
        ret = ESP_ERR_INVALID_ARG;
    }
    else
    {
        ret = ledc_set_duty(
            PWM_SPEED_MODE,
            PWM_CHANNEL,
            uDuty
        );

        if (ret != ESP_OK)
        {
            /* Duty-cycle configuration failed. */
        }
        else
        {
            ret = ledc_update_duty(
                PWM_SPEED_MODE,
                PWM_CHANNEL
            );
        }
    }

    return ret;
}