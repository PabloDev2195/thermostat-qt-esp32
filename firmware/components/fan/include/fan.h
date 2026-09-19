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
 * @brief Creates the FreeRTOS task responsible for managing the fan.
 */
void fan_task_create(void);

/**
 * @brief Updates the requested fan operating level.
 *
 * @param level Requested fan level.
 */
void fan_update(fan_level_t level);

#ifdef __cplusplus
}
#endif

#endif /* FAN_H */