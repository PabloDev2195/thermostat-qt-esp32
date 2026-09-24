#pragma once

#include <stdbool.h>

/**
 * Initializes NVS, NimBLE, and the GATT server, and starts BLE advertising
 * as "nimble-bleprph". Must be called once from app_main() before any
 * other BLE-related code runs.
 */
void ble_manager_init(void);

/**
 * Updates the current temperature value and notifies any connected
 * BLE client via the temperature characteristic. Safe to call even
 * when no client is connected (the notification is silently skipped).
 *
 * @param temperature_c Current temperature in degrees Celsius.
 */
void ble_manager_update_temperature(float temperature_c);

/**
 * @brief Updates the thermostat state and publishes it via BLE.
 *
 * Passes the new state to the GATT server for storage and
 * notification to the connected BLE client.
 *
 * @param[in] state New thermostat state value.
 *
 * @note Uses the BLE manager's current connection handle.
 */
void ble_manager_update_state(uint8_t state);

/**
 * Returns whether a BLE central is currently connected.
 */
bool ble_manager_is_connected(void);