/**
 * @file control.c
 * @brief Thermostat control logic implementation.
 *
 * @details
 * This module implements the thermostat control logic responsible for
 * controlling the heating and cooling outputs according to the current
 * temperature and the desired setpoint.
 *
 * The module uses GPIO abstraction functions to control the heater and
 * compressor. A FreeRTOS task periodically reads the current temperature
 * and evaluates the control conditions.
 *
 * Hysteresis is used to prevent rapid switching of the heating and cooling
 * outputs around the setpoint. The hysteresis value changes depending on
 * whether the setpoint has already been reached.
 *
 * @note
 * The control task currently uses a fixed setpoint of 25.0 degrees.
 * The control queue is created but is not currently used to process
 * commands.
 *
 * @warning
 * The implementation assumes that the temperature reading is valid and
 * that the GPIO functions successfully apply the requested output levels.
 */

/* Includes ----------------------------------------------------------------*/

#include "control.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "gpio.h"
#include "ble_mgr.h"
#include "temperature.h"
#include "fan.h"

/* Private definitions -----------------------------------------------------*/

/**
 * @brief Stack size allocated to the control task.
 */
#define CONTROL_TASK_STACK_SIZE    2048

/**
 * @brief Priority assigned to the control task.
 */
#define CONTROL_TASK_PRIORITY      6

/**
 * @brief Number of entries in the control command queue.
 */
#define CONTROL_QUEUE_LENGTH       1

/**
 * @brief Hysteresis applied after the setpoint has been reached.
 *
 * This value defines the temperature range around the setpoint during
 * normal thermostat operation.
 */
#define CONTROL_HYSTERESIS_NORMAL  1.5f

/**
 * @brief Hysteresis threshold for ECO mode.
 *
 * Defines the temperature hysteresis in degrees Celsius used
 * by the control logic when operating in ECO mode.
 */
#define CONTROL_HYSTERESIS_ECO     2.5f

/**
 * @brief Hysteresis applied before the setpoint has been reached.
 *
 * This value defines the temperature range around the setpoint while
 * the thermostat is approaching the desired temperature.
 */
#define CONTROL_HYSTERESIS_SETPOINT 0.1f

/* Private variables -------------------------------------------------------*/

/**
 * @brief Current hysteresis value used by the control algorithm.
 */
static float s_hysteresis = 0.5f;

/**
 * @brief Current setpoint value used by the control algorithm.
 */
static float fsetpoint = 22.0f;

/**
 * @brief Current operating state of the thermostat.
 *
 * The possible states represent heating, cooling, or no active output.
 */
static control_state_t s_state = CONTROL_STATE_IDLE;

static control_mode_t s_mode = CONTROL_MODE_OFF;
/* Private function prototypes --------------------------------------------*/

/**
 * @brief FreeRTOS task that periodically evaluates thermostat conditions.
 *
 * @param arg Task argument. Unused.
 */
static void control_task(void *arg);

/**
 * @brief Initialize the thermostat control module.
 *
 * @param void
 *
 * @return 0 on success, negative value on error.
 */
uint8_t control_init(void);

/* Public functions --------------------------------------------------------*/

/**
 * @brief Initializes the thermostat control module.
 *
 * Resets the operating mode and state to OFF and disables both the
 * heater and compressor outputs.
 *
 * @return 0 when initialization completes.
 *
 * @note
 * The function does not create the control task or command queue.
 */
uint8_t control_init(void)
{
    s_state = CONTROL_STATE_IDLE;
    s_mode = CONTROL_MODE_OFF;

    gpio_set_heater(GPIO_LEVEL_LOW);
    gpio_set_compressor(GPIO_LEVEL_LOW);

    return 0;
}

/* Private functions -------------------------------------------------------*/

/**
 * @brief FreeRTOS task that periodically updates thermostat control.
 *
 * Initializes the control module and enters an infinite loop. During
 * each iteration, the task reads the current temperature, evaluates
 * the control algorithm using a fixed setpoint, and waits for the
 * next update interval.
 *
 * @param arg Task argument. Unused.
 *
 * @note
 * The current implementation uses a fixed setpoint of 25.0 degrees
 * and an update interval of 300 milliseconds.
 *
 * @warning
 * This task does not currently check whether the temperature reading
 * is valid before passing it to control_update().
 */
static void control_task(void *arg)
{
    control_init();

    for(;;)
    {
        float current_temp = temperature_get_currentTemperature();

        control_update(current_temp, fsetpoint);

        vTaskDelay(pdMS_TO_TICKS(300));
    }
}

/* Public functions --------------------------------------------------------*/

/**
 * @brief Creates and starts the thermostat control task.
 *
 * Creates the FreeRTOS task responsible for periodically reading the
 * current temperature and updating the thermostat control state.
 *
 * @note
 * The control task runs periodically and evaluates the current
 * temperature against the configured setpoint.
 *
 * @warning
 * The return value of xTaskCreate() is not currently checked.
 * Therefore, this function does not report task creation failures.
 */
void control_task_create(void)
{
    xTaskCreate(
        control_task,
        "control_task",
        CONTROL_TASK_STACK_SIZE,
        NULL,
        CONTROL_TASK_PRIORITY,
        NULL
    );
}

/**
 * @brief Evaluates thermostat conditions and updates the outputs.
 *
 * Compares the current temperature against upper and lower limits
 * calculated from the setpoint and the active hysteresis value.
 *
 * The hysteresis is selected according to whether the setpoint was
 * reached during the previous evaluation:
 *
 * - Before the setpoint is reached, CONTROL_HYSTERESIS_SETPOINT is used.
 * - After the setpoint is reached, CONTROL_HYSTERESIS_NORMAL is used.
 *
 * The heater is enabled when the current temperature is below the
 * lower limit. The compressor is enabled when the current temperature
 * is above the upper limit. Both outputs are disabled when the
 * temperature is within the limits.
 *
 * @param current_temperature Current measured temperature.
 * @param setpoint Desired thermostat temperature.
 *
 * @return 0 when the control evaluation completes.
 *
 * @note
 * The setpoint-reached flag is retained between calls using a static
 * local variable.
 *
 * @warning
 * The operating mode variable is not currently checked by this function.
 * Consequently, the control algorithm can activate heating or cooling
 * even when s_mode is CONTROL_MODE_OFF.
 */
uint8_t control_update(float current_temperature, float setpoint)
{
    static bool bsetpointReached = false;
    static control_state_t state = CONTROL_STATE_IDLE;

    if(s_mode == CONTROL_MODE_OFF)
    {
        bsetpointReached = false;
        s_state = CONTROL_STATE_IDLE;
        gpio_set_heater(GPIO_LEVEL_LOW);
        gpio_set_compressor(GPIO_LEVEL_LOW);
        fan_update(FAN_LEVEL_OFF);
    }
    else
    {
        if(bsetpointReached)
        {
            if(s_mode == CONTROL_MODE_ECO)
            {
                s_hysteresis = CONTROL_HYSTERESIS_ECO;
            }
            else
            {
                s_hysteresis = CONTROL_HYSTERESIS_NORMAL;
            }
        }
        else
        {
            s_hysteresis = CONTROL_HYSTERESIS_SETPOINT;
        }

        const float upper_limit = setpoint + s_hysteresis;
        const float lower_limit = setpoint - s_hysteresis;

        if(current_temperature < lower_limit)
        {
            gpio_set_compressor(GPIO_LEVEL_LOW);
            gpio_set_heater(GPIO_LEVEL_HIGH);
            fan_update(fan_get_level());

            s_state = CONTROL_STATE_HEAT;
            bsetpointReached = false;
        }
        else if(current_temperature > upper_limit)
        {
            gpio_set_heater(GPIO_LEVEL_LOW);
            gpio_set_compressor(GPIO_LEVEL_HIGH);
            fan_update(fan_get_level());

            s_state = CONTROL_STATE_COOL;
            bsetpointReached = false;
        }
        else
        {
            gpio_set_heater(GPIO_LEVEL_LOW);
            gpio_set_compressor(GPIO_LEVEL_LOW);
            fan_update(FAN_LEVEL_OFF);

            s_state = CONTROL_STATE_IDLE;
            bsetpointReached = true;
        }
    }

    if(state != s_state)
    {
        state = s_state;
        ble_manager_update_state((uint8_t)s_state);
    }

    return 0;
}

/**
 * @brief Gets the current thermostat control state.
 *
 * Returns the state most recently assigned by control_init() or
 * control_update().
 *
 * @return Current control state:
 *         - CONTROL_STATE_OFF: Heating and cooling are inactive.
 *         - CONTROL_STATE_HEAT: Heating is active.
 *         - CONTROL_STATE_COOL: Cooling is active.
 */
control_state_t control_get_state(void)
{
    return s_state;
}

/**
 * @brief Gets the current operating mode of the control module.
 *
 * Returns the operating mode currently stored in the control module.
 *
 * @return Current control operating mode (control_mode_t).
 */
control_mode_t control_get_mode(void)
{
    return s_mode;
}

/**
 * @brief Updates the thermostat setpoint.
 *
 * Converts the setpoint value received as an unsigned 16-bit integer,
 * expressed in tenths of a degree Celsius, into a floating-point
 * temperature in degrees Celsius and stores it in `fsetpoint`.
 *
 * @param[in] setpoint Setpoint temperature multiplied by 10.
 *                     For example, 220 represents 22.0 °C.
 */
void control_set_setpoint(uint16_t setpoint)
{
    fsetpoint = setpoint/10.0f;
}

/**
 * @brief Sets the operating mode of the control module.
 *
 * Updates the current control mode by converting the provided
 * value to the corresponding control_mode_t enumeration.
 *
 * @param[in] mode Operating mode value corresponding to control_mode_t.
 *
 * @note The input value must match a valid control_mode_t enumerator.
 */
void control_set_mode(uint8_t mode)
{
    s_mode = (control_mode_t)mode;
}