/**
 * @file esp32_gpio.c
 * @brief ESP32 GPIO hardware abstraction layer implementation.
 *
 * @details
 * This module provides ESP32-specific GPIO functions used by the
 * generic GPIO abstraction layer.
 *
 * It encapsulates the ESP-IDF GPIO API, allowing higher-level modules
 * to configure, write, and read GPIO pins without directly depending
 * on ESP-IDF GPIO functions.
 */

#include "esp32_gpio.h"

/**
 * @brief Configures an ESP32 GPIO as a digital output.
 *
 * Configures the specified GPIO pin as an output with pull-up,
 * pull-down, and interrupt functionality disabled. After the GPIO
 * configuration is completed successfully, the pin is initialized
 * to the requested output level.
 *
 * @param pin GPIO pin to configure.
 * @param initial_level Initial output level to apply to the GPIO.
 *
 * @return ESP_OK if the GPIO is configured and initialized successfully.
 * @return An ESP-IDF error code if GPIO configuration or initialization
 *         fails.
 *
 * @note
 * The function uses the ESP-IDF GPIO driver internally.
 */
esp_err_t esp32_gpio_init_output(gpio_num_t pin, uint32_t initial_level)
{
    esp_err_t err;
    
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << pin),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };

    err = gpio_config(&io_conf);

    if (err != ESP_OK)
    {
        
    }
    else
    {
        err = gpio_set_level(pin, initial_level);
    }

    return err;
}

/**
 * @brief Sets the output level of an ESP32 GPIO.
 *
 * Writes the specified digital level to the selected GPIO pin.
 *
 * @param pin GPIO pin to update.
 * @param level Output level to apply to the GPIO.
 *
 * @return ESP_OK if the GPIO level is set successfully.
 * @return An ESP-IDF error code if the operation fails.
 */
esp_err_t esp32_gpio_set_level(gpio_num_t pin, uint32_t level)
{
    return gpio_set_level(pin, level);
}

/**
 * @brief Reads the current level of an ESP32 GPIO.
 *
 * Returns the digital logic level currently detected on the
 * specified GPIO pin.
 *
 * @param pin GPIO pin to read.
 *
 * @return GPIO level read from the specified pin.
 */
int esp32_gpio_get_level(gpio_num_t pin)
{
    return gpio_get_level(pin);
}