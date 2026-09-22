/**
 * @file gpio.h
 * @brief GPIO hardware abstraction interface.
 */

#ifndef GPIO_H
#define GPIO_H

#include <stdint.h>

typedef enum
{
    GPIO_LEVEL_LOW = 0,
    GPIO_LEVEL_HIGH = 1
} gpio_level_t;

/**
 * @brief Initialize all application GPIO outputs.
 *
 * Initializes the heater and compressor GPIOs with
 * their default inactive state.
 *
 * @return 0 on success, negative value on error.
 */
int gpio_init(void);

/**
 * @brief Set heater GPIO level.
 *
 * @param level GPIO logic level.
 *
 * @return 0 on success, negative value on error.
 */
int gpio_set_heater(gpio_level_t level);

/**
 * @brief Set compressor GPIO level.
 *
 * @param level GPIO logic level.
 *
 * @return 0 on success, negative value on error.
 */
int gpio_set_compressor(gpio_level_t level);

/**
 * @brief Get heater GPIO level.
 *
 * @return Current GPIO logic level.
 */
gpio_level_t gpio_get_heater(void);

/**
 * @brief Get compressor GPIO level.
 *
 * @return Current GPIO logic level.
 */
gpio_level_t gpio_get_compressor(void);

#endif /* GPIO_H */