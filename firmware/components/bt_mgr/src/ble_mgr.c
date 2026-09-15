#include "esp_log.h"
#include "nvs_flash.h"
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "host/ble_hs.h"
#include "host/util/util.h"
#include "services/gap/ble_svc_gap.h"
#include "esp32_bleprph.h"
#include "ble_mgr.h"

static const char *tag = "BLE_MGR";
static int bleprph_gap_event(struct ble_gap_event *event, void *arg);
static uint8_t own_addr_type;
static uint16_t g_conn_handle = BLE_HS_CONN_HANDLE_NONE;

void ble_store_config_init(void);

/**
 * @brief Logs details of an active BLE connection.
 * @param desc Connection descriptor (handle, OTA address type and value).
 *
 * Debug helper: called whenever a connection is successfully established,
 * to record the handle and peer address in the log.
 */
static void bleprph_print_conn_desc(struct ble_gap_conn_desc *desc)
{
    MODLOG_DFLT(INFO, "handle=%d our_ota_addr_type=%d our_ota_addr=",
                desc->conn_handle, desc->our_ota_addr.type);
    print_addr(desc->our_ota_addr.val);
    MODLOG_DFLT(INFO, "\n");
}

/**
 * @brief Configures the advertising data and starts BLE advertising.
 *
 * Builds the advertising packet with:
 *  - General discoverable mode flags + BLE-only (BR/EDR unsupported).
 *  - TX power level (automatic).
 *  - Device name (whatever was set via `ble_svc_gap_device_name_set`,
 *    here "nimble-bleprph").
 *  - Alert service UUID (`GATT_SVR_SVC_ALERT_UUID`).
 *
 * Then calls `ble_gap_adv_start()` in undirected/general-discoverable mode
 * so the ESP32 is visible and connectable by a central (the Qt app) at any
 * time. It is automatically called again whenever a connection ends or
 * advertising times out, so the device is effectively always advertising.
 */
static void bleprph_advertise(void)
{
    struct ble_gap_adv_params adv_params;
    struct ble_hs_adv_fields fields;
    const char *name;
    int rc;

    memset(&fields, 0, sizeof fields);
    fields.flags = BLE_HS_ADV_F_DISC_GEN | BLE_HS_ADV_F_BREDR_UNSUP;
    fields.tx_pwr_lvl_is_present = 1;
    fields.tx_pwr_lvl = BLE_HS_ADV_TX_PWR_LVL_AUTO;

    name = ble_svc_gap_device_name();
    fields.name = (uint8_t *)name;
    fields.name_len = strlen(name);
    fields.name_is_complete = 1;

    fields.uuids16 = (ble_uuid16_t[]) { BLE_UUID16_INIT(GATT_SVR_SVC_ALERT_UUID) };
    fields.num_uuids16 = 1;
    fields.uuids16_is_complete = 1;

    rc = ble_gap_adv_set_fields(&fields);
    if (rc != 0) 
    {
        MODLOG_DFLT(ERROR, "error setting advertisement data; rc=%d\n", rc);
    }
    else
    {
        memset(&adv_params, 0, sizeof adv_params);
        adv_params.conn_mode = BLE_GAP_CONN_MODE_UND;
        adv_params.disc_mode = BLE_GAP_DISC_MODE_GEN;
        rc = ble_gap_adv_start(own_addr_type, NULL, BLE_HS_FOREVER,
                            &adv_params, bleprph_gap_event, NULL);
        if (rc != 0) 
        {
            MODLOG_DFLT(ERROR, "error enabling advertisement; rc=%d\n", rc);
        }
    }
}

/**
 * @brief Central NimBLE GAP event callback.
 * @param event GAP event that occurred (connect, disconnect, MTU, etc.).
 * @param arg   Application argument (unused here).
 * @return Always 0; the return value is ignored for most event types.
 *
 * NimBLE invokes this function to report any event related to the BLE link.
 * Handled cases:
 *  - `BLE_GAP_EVENT_CONNECT`: logs the outcome; on success prints the
 *    connection descriptor, on failure restarts advertising.
 *  - `BLE_GAP_EVENT_DISCONNECT`: logs the reason and restarts advertising
 *    so the device remains discoverable.
 *  - `BLE_GAP_EVENT_ADV_COMPLETE`: advertising ended (timeout); restarted
 *    so the device never stops advertising.
 *  - `BLE_GAP_EVENT_SUBSCRIBE`: a central subscribed/unsubscribed from
 *    notifications on a GATT characteristic; logged only.
 *  - `BLE_GAP_EVENT_MTU`: a new MTU size was negotiated; logged only.
 *
 * Any other event type not listed here is silently ignored (falls through
 * to the final `return 0`).
 */
static int bleprph_gap_event(struct ble_gap_event *event, void *arg)
{
    struct ble_gap_conn_desc desc;
    int rc;

    switch (event->type) 
    {
        case BLE_GAP_EVENT_CONNECT:
            MODLOG_DFLT(INFO, "connection %s; status=%d\n",
                        event->connect.status == 0 ? "established" : "failed",
                        event->connect.status);
            if (event->connect.status == 0) 
            {
                g_conn_handle = event->connect.conn_handle;   // 👈 agrega esta línea
                rc = ble_gap_conn_find(event->connect.conn_handle, &desc);
                assert(rc == 0);
                bleprph_print_conn_desc(&desc);
            } 
            else 
            {
                bleprph_advertise();
            }
            break;

        case BLE_GAP_EVENT_DISCONNECT:
            MODLOG_DFLT(INFO, "disconnect; reason=%d\n", event->disconnect.reason);
            g_conn_handle = BLE_HS_CONN_HANDLE_NONE;   // 👈 agrega esta línea
            bleprph_advertise();
            break;

        case BLE_GAP_EVENT_ADV_COMPLETE:
            bleprph_advertise();
            break;

        case BLE_GAP_EVENT_SUBSCRIBE:
            MODLOG_DFLT(INFO, "subscribe event; conn_handle=%d attr_handle=%d\n",
                        event->subscribe.conn_handle, event->subscribe.attr_handle);
            break;

        case BLE_GAP_EVENT_MTU:
            MODLOG_DFLT(INFO, "mtu update event; mtu=%d\n", event->mtu.value);
            break;
    }
    return 0;
}

/**
 * @brief Callback invoked by NimBLE when the host unexpectedly resets.
 * @param reason Reset reason code provided by the NimBLE stack.
 *
 * Only logs the event as an error; it does not attempt any automatic
 * recovery (e.g. rebooting the ESP32 or retrying initialization).
 */
static void bleprph_on_reset(int reason)
{
    MODLOG_DFLT(ERROR, "Resetting state; reason=%d\n", reason);
}

/**
 * @brief Callback invoked by NimBLE once the host has synced (started) successfully.
 *
 * Runs once at startup, after the BLE stack is ready to operate. It:
 *  1. Ensures a valid identity address exists (`ble_hs_util_ensure_addr`),
 *     preferring a public address.
 *  2. Determines which own-address type to use for advertising
 *     (`ble_hs_id_infer_auto`).
 *  3. Retrieves and logs the device's MAC address, useful for debugging
 *     which physical ESP32 is running the firmware.
 *  4. Calls `bleprph_advertise()` to start advertising for the first time.
 *
 * If determining the address type fails, it logs the error and returns
 * without starting advertising (the device would remain non-discoverable
 * until a restart).
 */
static void bleprph_on_sync(void)
{
    int rc;

    rc = ble_hs_util_ensure_addr(0);
    assert(rc == 0);

    rc = ble_hs_id_infer_auto(0, &own_addr_type);
    if (rc != 0) {
        MODLOG_DFLT(ERROR, "error determining address type; rc=%d\n", rc);
        return;
    }

    uint8_t addr_val[6] = {0};
    ble_hs_id_copy_addr(own_addr_type, addr_val, NULL);
    MODLOG_DFLT(INFO, "Device Address: ");
    print_addr(addr_val);
    MODLOG_DFLT(INFO, "\n");

    bleprph_advertise();
}

/**
 * @brief FreeRTOS task that runs the main NimBLE host loop.
 * @param param FreeRTOS task parameter (unused).
 *
 * Automatically created by `nimble_port_freertos_init()` inside
 * `ble_manager_init()`. `nimble_port_run()` blocks indefinitely processing
 * BLE stack events — the function only returns once `nimble_port_stop()`
 * is called (explicit stack shutdown, not currently used in this project).
 * On return, it cleans up the FreeRTOS resources tied to the NimBLE port
 * via `nimble_port_freertos_deinit()`.
 */
static void bleprph_host_task(void *param)
{
    ESP_LOGI(tag, "BLE Host Task Started");
    nimble_port_run();
    nimble_port_freertos_deinit();
}

/**
 * @brief Initializes NVS, the NimBLE stack, and the GATT server, and starts BLE advertising.
 *
 * Public entry point of the `ble_mgr` module. Must be called exactly once
 * from `app_main()` before any other BLE-related code runs. Steps:
 *
 *  1. **NVS**: initializes non-volatile storage (required by NimBLE to
 *     store PHY calibration data and bonding info). If it detects
 *     corrupted pages or an incompatible version, erases and reinitializes.
 *  2. **NimBLE port**: initializes the NimBLE port on top of ESP-IDF
 *     (`nimble_port_init`). On failure, logs the error and returns without
 *     continuing (the rest of BLE initialization does not run).
 *  3. **Host configuration**: assigns the reset, sync, and GATT-registration
 *     callbacks to NimBLE's global `ble_hs_cfg`.
 *  4. **GATT server**: initializes the services/characteristics defined in
 *     `gatt_svr.c` via `gatt_svr_init()`.
 *  5. **Device name**: sets the advertising name to "nimble-bleprph" — this
 *     is the name the Qt/QML `BleManager` filters on to identify the ESP32.
 *  6. **Store config**: initializes BLE bonding/key storage.
 *  7. **FreeRTOS task**: creates the `bleprph_host_task` task that runs the
 *     NimBLE host loop indefinitely, running alongside any other task
 *     created in `app_main()` (e.g. the thermostat control task).
 */
void ble_manager_init(void)
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ret = nimble_port_init();
    if (ret != ESP_OK) {
        ESP_LOGE(tag, "Failed to init nimble %d", ret);
        return;
    }

    ble_hs_cfg.reset_cb = bleprph_on_reset;
    ble_hs_cfg.sync_cb = bleprph_on_sync;
    ble_hs_cfg.gatts_register_cb = gatt_svr_register_cb;
    ble_hs_cfg.store_status_cb = ble_store_util_status_rr;

    int rc = gatt_svr_init();
    assert(rc == 0);

    rc = ble_svc_gap_device_name_set("nimble-bleprph");
    assert(rc == 0);

    ble_store_config_init();
    nimble_port_freertos_init(bleprph_host_task);
}

/**
 * @brief Update the temperature value exposed through BLE.
 *
 * Updates the temperature characteristic with the provided temperature
 * value for the currently connected BLE client.
 *
 * @param[in] temperature_c Temperature value in degrees Celsius.
 */
void ble_manager_update_temperature(float temperature_c)
{
    gatt_svr_set_temperature(temperature_c, g_conn_handle);
}

/**
 * @brief Check whether a BLE client is currently connected.
 *
 * Checks the current BLE connection handle to determine whether
 * a client is connected to the device.
 *
 * @return
 * - true if a BLE client is connected.
 * - false if there is no active BLE connection.
 */
bool ble_manager_is_connected(void)
{
    return g_conn_handle != BLE_HS_CONN_HANDLE_NONE;
}