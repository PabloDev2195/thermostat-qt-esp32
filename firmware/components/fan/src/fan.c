/**
 * @file fan.c
 * @brief Generic fan control implementation using PWM.
 *
 * Maps abstract fan operating levels to PWM duty-cycle values.
 */

#include "fan.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "pwm.h"

#define FAN_TASK_STACK_SIZE    2048
#define FAN_TASK_PRIORITY      3
#define FAN_QUEUE_LENGTH       1

/**
 * @brief Queue used to receive fan level update requests.
 *
 * The queue stores the latest requested fan operating level.
 */
static QueueHandle_t fan_queue = NULL;

/**
 * @brief PWM duty cycle for the fan OFF state.
 */
#define FAN_DUTY_OFF       0

/**
 * @brief PWM duty cycle for the fan LOW state.
 *
 * Corresponds to approximately 20% duty cycle.
 */
#define FAN_DUTY_LOW       20

/**
 * @brief PWM duty cycle for the fan MEDIUM state.
 *
 * Corresponds to approximately 55% duty cycle.
 */
#define FAN_DUTY_MEDIUM    55

/**
 * @brief PWM duty cycle for the fan HIGH state.
 *
 * Corresponds to 100% duty cycle.
 */
#define FAN_DUTY_HIGH      100

/**
 * @brief Current fan operating level.
 *
 * The fan starts in the OFF state.
 */
static fan_level_t current_level = FAN_LEVEL_LOW;

/**
 * @brief Sets the fan operating level.
 *
 * @param level Desired fan operating level.
 *
 * @return Status indicating whether the operation succeeded.
 */
fan_status_t fan_set_level(fan_level_t level);

/**
 * @brief Initializes the fan module.
 *
 * Initializes the underlying PWM module and sets the fan
 * to the OFF state.
 *
 * @return
 * - FAN_OK if the fan was initialized successfully.
 * - FAN_ERROR if PWM initialization or configuration fails.
 */
fan_status_t fan_init(void);

/**
 * @brief Initializes the fan module.
 *
 * Sets the fan PWM duty cycle to the OFF state and updates
 * the current operating level when successful.
 *
 * @return
 * - FAN_OK if initialization succeeds.
 * - FAN_ERROR if setting the PWM duty cycle fails.
 */
fan_status_t fan_init(void)
{
    fan_status_t status = FAN_OK;

    if (pwm_set_duty(FAN_DUTY_OFF) != PWM_OK)
    {
        status = FAN_ERROR;
    }
    else
    {
        current_level = FAN_LEVEL_LOW;
    }
    return status;
}

/**
 * @brief FreeRTOS task that processes fan level update requests.
 *
 * Initializes the fan and waits for level commands from the
 * fan queue. Each received command is passed to fan_set_level().
 *
 * @param arg Task argument (unused).
 */
static void fan_task(void *arg)
{
    fan_level_t level;
    fan_init();

    for(;;)
    {
        if (xQueueReceive(fan_queue, &level, portMAX_DELAY) == pdTRUE)
        {
            fan_set_level(level);
        }
    }
}

/**
 * @brief Creates the fan command queue and control task.
 *
 * The queue stores fan level commands, while the FreeRTOS task
 * processes the commands and applies the corresponding PWM output.
 */
void fan_task_create(void)
{
    fan_queue = xQueueCreate(
        FAN_QUEUE_LENGTH,
        sizeof(fan_level_t)
    );

    xTaskCreate(
        fan_task,
        "fan_task",
        FAN_TASK_STACK_SIZE,
        NULL,
        FAN_TASK_PRIORITY,
        NULL
    );
}

/**
 * @brief Sets the fan operating level.
 *
 * Converts the requested fan level into the corresponding
 * PWM duty cycle and applies it through the generic PWM interface.
 *
 * @param[in] level Desired fan operating level.
 *
 * @return
 * - FAN_OK if the fan level was changed successfully.
 * - FAN_INVALID_LEVEL if the specified level is invalid.
 * - FAN_ERROR if the PWM operation fails.
 */
fan_status_t fan_set_level(fan_level_t level)
{
    uint8_t duty = FAN_DUTY_OFF;
    fan_status_t status = FAN_OK;

    switch (level)
    {
        case FAN_LEVEL_OFF:
            duty = FAN_DUTY_OFF;
            break;

        case FAN_LEVEL_LOW:
            duty = FAN_DUTY_LOW;
            break;

        case FAN_LEVEL_MEDIUM:
            duty = FAN_DUTY_MEDIUM;
            break;

        case FAN_LEVEL_HIGH:
            duty = FAN_DUTY_HIGH;
            break;

        default:
            status = FAN_INVALID_LEVEL;
    }

    if (pwm_set_duty(duty) != PWM_OK)
    {
        status = FAN_ERROR;
    }
    else
    {
        if(level != FAN_LEVEL_OFF)
        {
            current_level = level;
        } 
    }

    return status;
}

/**
 * @brief Submits a fan level update request.
 *
 * Sends the requested operating level to the fan queue for
 * processing by the fan control task. The queue retains the
 * latest requested level.
 *
 * @param level Requested fan operating level.
 */
void fan_update(fan_level_t level)
{
    if (fan_queue != NULL)
    {
        xQueueOverwrite(fan_queue, &level);
    }
}

/**
 * @brief Gets the current fan operating level.
 *
 * Retrieves the fan level currently stored in the control module.
 *
 * @return Current fan operating level (fan_level_t).
 */
fan_level_t fan_get_level(void)
{
    return current_level;
}