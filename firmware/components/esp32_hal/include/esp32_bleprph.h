/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#ifndef H_ESP32_BLEPRPH_
#define H_ESP32_BLEPRPH_

#include <stdbool.h>
#include "nimble/ble.h"
#include "modlog/modlog.h"
#include "esp_peripheral.h"
#ifdef __cplusplus
extern "C" {
#endif

struct ble_hs_cfg;
struct ble_gatt_register_ctxt;

/** GATT server. */
#define GATT_SVR_SVC_ALERT_UUID               0x1811
#define GATT_SVR_CHR_SUP_NEW_ALERT_CAT_UUID   0x2A47
#define GATT_SVR_CHR_NEW_ALERT                0x2A46
#define GATT_SVR_CHR_SUP_UNR_ALERT_CAT_UUID   0x2A48
#define GATT_SVR_CHR_UNR_ALERT_STAT_UUID      0x2A45
#define GATT_SVR_CHR_ALERT_NOT_CTRL_PT        0x2A44

/**
 * @brief Callback type for handling fan level updates received via GATT.
 *
 * @param level Fan level received from the GATT service.
 */
typedef void (*gatt_svr_fan_level_callback_t)(uint8_t level);

/**
 * @brief Callback type for handling fan level updates received via GATT.
 *
 * @param level Fan level received from the GATT service.
 */
typedef void (*gatt_svr_setpoint_callback_t)(uint16_t setpoint);

/**
 * @brief Callback type for handling mode updates received via GATT.
 *
 * @param mode Mode received from the GATT service.
 */
typedef void (*gatt_svr_mode_callback_t)(uint8_t mode);

/**
 * @brief GATT server registration callback.
 *
 * Called by the NimBLE GATT server when a service, characteristic,
 * or descriptor is registered.
 *
 * @param[in] ctxt Registration context provided by the NimBLE stack.
 * @param[in] arg User-defined argument provided during registration.
 */
void gatt_svr_register_cb(struct ble_gatt_register_ctxt *ctxt, void *arg);

/**
 * @brief Initialize the GATT server.
 *
 * Registers the GATT services, characteristics, and descriptors
 * required by the BLE application.
 *
 * @return
 * - 0 if the GATT server was initialized successfully.
 * - A non-zero error code if initialization failed.
 */
int gatt_svr_init(void);

/**
 * @brief Update the temperature characteristic.
 *
 * Updates the temperature value associated with the connected BLE client.
 *
 * @param[in] temperature_c Temperature value in degrees Celsius.
 * @param[in] conn_handle BLE connection handle of the client.
 */
void gatt_svr_set_temperature(float temperature_c, uint16_t conn_handle);

/**
 * @brief Sets the callback used to handle fan level updates.
 *
 * @param callback Callback function invoked when a fan level is received.
 */
void gatt_svr_set_fan_level_callback(gatt_svr_fan_level_callback_t callback);

/**
 * @brief Sets the callback used to handle setpoint values updates.
 *
 * @param callback Callback function invoked when a setpoint value is received.
 */
void gatt_svr_set_setpoint_callback(gatt_svr_setpoint_callback_t callback);

/**
 * @brief Sets the callback used to handle mode updates.
 *
 * @param callback Callback function invoked when a mode value is received.
 */
void gatt_svr_set_mode_callback(gatt_svr_mode_callback_t callback);

#ifdef __cplusplus
}
#endif

#endif
