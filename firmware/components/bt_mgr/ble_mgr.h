#pragma once

/**
 * Initializes NVS, NimBLE, and the GATT server, and starts BLE advertising
 * as "nimble-bleprph". Must be called once from app_main() before any
 * other BLE-related code runs.
 */
void ble_manager_init(void);