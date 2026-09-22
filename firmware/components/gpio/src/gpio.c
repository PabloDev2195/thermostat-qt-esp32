/**
 * @file gpio.c
 * @brief Generic GPIO control implementation for thermostat outputs.
 *
 * @details
 * This module provides a hardware-independent interface for controlling
 * the thermostat heater and compressor outputs.
 *
 * The module uses the ESP32 GPIO abstraction layer internally to configure
 * and control the physical GPIO pins. This keeps ESP32-specific hardware
 * details isolated from the generic GPIO interface.
 *
 * Heater:
 * - GPIO25
 *
 * Compressor:
 * - GPIO32
 *
 * Both outputs are initialized to GPIO_LEVEL_LOW to ensure that the
 * heater and compressor are disabled during system startup.
 */

#include "gpio.h"
#include "esp32_gpio.h"

/* Private definitions -----------------------------------------------------*/

/**
 * @brief GPIO pin assigned to the heater output.
 */
#define HEATER_GPIO      25

/**
 * @brief GPIO pin assigned to the compressor output.
 */
#define COMPRESSOR_GPIO  32

/* Public functions --------------------------------------------------------*/

/**
 * @brief Initializes the thermostat GPIO outputs.
 *
 * Configures the heater and compressor GPIOs as digital outputs and
 * initializes both outputs to GPIO_LEVEL_LOW.
 *
 * @return 0 if both GPIOs are initialized successfully.
 * @return Non-zero error code if initialization of either GPIO fails.
 *
 * @note
 * The heater is connected to GPIO25 and the compressor is connected
 * to GPIO32.
 */
int gpio_init(void)
{
    int err = 0;

    err = esp32_gpio_init_output(HEATER_GPIO,GPIO_LEVEL_LOW);

    if (err == 0)
    {
        err = esp32_gpio_init_output(COMPRESSOR_GPIO,GPIO_LEVEL_LOW);
    }

    return err;
}

/**
 * @brief Sets the heater output level.
 *
 * Updates the digital output associated with the heater.
 *
 * @param level Desired GPIO output level.
 *
 * @return 0 if the output is updated successfully.
 * @return Non-zero error code if the GPIO operation fails.
 */
int gpio_set_heater(gpio_level_t level)
{
    return esp32_gpio_set_level(
        HEATER_GPIO,
        level
    );
}

/**
 * @brief Sets the compressor output level.
 *
 * Updates the digital output associated with the compressor.
 *
 * @param level Desired GPIO output level.
 *
 * @return 0 if the output is updated successfully.
 * @return Non-zero error code if the GPIO operation fails.
 */
int gpio_set_compressor(gpio_level_t level)
{
    return esp32_gpio_set_level(
        COMPRESSOR_GPIO,
        level
    );
}

/**
 * @brief Gets the current heater output level.
 *
 * Reads the digital level currently applied to the heater GPIO.
 *
 * @return Current heater GPIO level.
 */
gpio_level_t gpio_get_heater(void)
{
    return (gpio_level_t)esp32_gpio_get_level(HEATER_GPIO);
}

/**
 * @brief Gets the current compressor output level.
 *
 * Reads the digital level currently applied to the compressor GPIO.
 *
 * @return Current compressor GPIO level.
 */
gpio_level_t gpio_get_compressor(void)
{
    return (gpio_level_t)esp32_gpio_get_level(COMPRESSOR_GPIO);
}