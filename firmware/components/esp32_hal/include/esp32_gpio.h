
/**
 * @file esp32_gpio.h
 * @brief ESP32 GPIO hardware abstraction layer.
 */

#ifndef ESP32_GPIO_H
#define ESP32_GPIO_H

#include <stdbool.h>
#include "esp_err.h"
#include "driver/gpio.h"

/**
 * @brief Initializes a GPIO pin as a digital output.
 *
 * @param pin GPIO pin number.
 * @param initial_level Initial output level (0 = LOW, 1 = HIGH).
 *
 * @return ESP_OK on success, otherwise an error code.
 */
esp_err_t esp32_gpio_init_output(gpio_num_t pin, uint32_t initial_level);

/**
 * @brief Sets the output level of a GPIO pin.
 *
 * @param pin GPIO pin number.
 * @param level Output level (0 = LOW, 1 = HIGH).
 *
 * @return ESP_OK on success, otherwise an error code.
 */
esp_err_t esp32_gpio_set_level(gpio_num_t pin, uint32_t level);

/**
 * @brief Reads the current level of a GPIO pin.
 *
 * @param pin GPIO pin number.
 *
 * @return GPIO input level (0 = LOW, 1 = HIGH).
 */
int esp32_gpio_get_level(gpio_num_t pin);

#endif /* ESP32_GPIO_H */