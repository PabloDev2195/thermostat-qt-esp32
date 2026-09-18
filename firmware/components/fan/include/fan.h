/**
 * @file fan.h
 * @brief Generic fan control interface.
 *
 * Provides an abstract interface for initializing the fan,
 * selecting its operating level, and reading the current level.
 */

#ifndef FAN_H
#define FAN_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Fan operating levels.
 *
 * Defines the available fan operating states.
 */
typedef enum
{
    /**
     * @brief Fan turned off.
     */
    FAN_LEVEL_OFF = 0,

    /**
     * @brief Fan operating at low speed.
     */
    FAN_LEVEL_LOW,

    /**
     * @brief Fan operating at medium speed.
     */
    FAN_LEVEL_MEDIUM,

    /**
     * @brief Fan operating at high speed.
     */
    FAN_LEVEL_HIGH

} fan_level_t;

/**
 * @brief Fan operation status.
 */
typedef enum
{
    /**
     * @brief Operation completed successfully.
     */
    FAN_OK = 0,

    /**
     * @brief Generic fan error.
     */
    FAN_ERROR,

    /**
     * @brief Invalid fan level.
     */
    FAN_INVALID_LEVEL

} fan_status_t;

/**
 * @brief Initializes the fan module.
 *
 * Initializes the underlying PWM interface and sets the fan
 * to the OFF state.
 *
 * @return
 * - FAN_OK if the fan was initialized successfully.
 * - FAN_ERROR if the PWM initialization or configuration failed.
 */
fan_status_t fan_init(void);

/**
 * @brief Sets the fan operating level.
 *
 * Changes the fan speed according to the selected operating level.
 * The corresponding PWM duty cycle is handled internally by the
 * fan module.
 *
 * @param[in] level Desired fan operating level.
 *
 * @return
 * - FAN_OK if the fan level was changed successfully.
 * - FAN_INVALID_LEVEL if the specified level is not valid.
 * - FAN_ERROR if the underlying PWM operation failed.
 */
fan_status_t fan_set_level(fan_level_t level);

/**
 * @brief Gets the current fan operating level.
 *
 * @return Current fan operating level.
 */
fan_level_t fan_get_level(void);

#ifdef __cplusplus
}
#endif

#endif /* FAN_H */