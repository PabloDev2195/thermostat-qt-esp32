#pragma once
#include <QObject>
#include <QBluetoothDeviceDiscoveryAgent>
#include <QLowEnergyController>
#include <QLowEnergyService>

/**
 * @brief Manages BLE discovery and connection to the ESP32 (NimBLE firmware).
 *
 * BleManager wraps QBluetoothDeviceDiscoveryAgent and QLowEnergyController to:
 *  - Scan for nearby BLE devices.
 *  - Filter and automatically connect to the device advertising as
 *    "nimble-bleprph" (the project's ESP32 thermostat).
 *  - Expose the connection state as a Qt property (`connected`) consumable
 *    directly from QML.
 */
class BleManager : public QObject {
    Q_OBJECT

    /**
     * @brief BLE connection status.
     *
     * Exposes the current BLE connection state to QML as a read-only property.
     * The value is updated when the BLE connection state changes.
     *
     * The NOTIFY signal allows QML to automatically update bindings when
     * the connection status changes.
     */
    Q_PROPERTY(bool connected READ connected NOTIFY connectedChanged)

    /**
     * @brief Current temperature received from the BLE device.
     *
     * Exposes the current temperature to QML as a read-only property.
     * The value is updated when new temperature data is received through
     * the BLE characteristic.
     *
     * The NOTIFY signal allows QML to automatically update bindings when
     * the temperature value changes.
     */
    Q_PROPERTY(double currentTemp READ currentTemp NOTIFY currentTempChanged)

public:
    /**
     * @brief Constructs the manager and prepares the BLE discovery agent.
     * @param parent Parent object in the QObject hierarchy (Qt memory management).
     */
    explicit BleManager(QObject* parent = nullptr);

    /**
     * @brief Getter for the `connected` property.
     * @return true if a GATT connection to the device is currently established.
     */
    bool connected() const { return m_connected; }

    /**
     * @brief Starts scanning for BLE devices.
     *
     * Exposed to QML via Q_INVOKABLE (e.g. called from a "Connect" / "Scan"
     * button). Internally restricts the scan to LowEnergyMethod only.
     */
    Q_INVOKABLE void startScan();

    /**
     * @brief Get the current temperature.
     *
     * @return Current temperature in degrees Celsius.
     */
    double currentTemp() const { return m_currentTemp; }

signals:
    /**
     * @brief Emitted when the BLE connection status changes.
     */
    void connectedChanged();

    /**
     * @brief Emitted when the current temperature changes.
     */
    void currentTempChanged();

private slots:
    /**
     * @brief Handles each BLE device found during scanning.
     * @param device Info about the discovered device (name, address, RSSI, etc.).
     *
     * If device.name() matches "nimble-bleprph", stops the scan, creates the
     * central QLowEnergyController for that device, wires up its
     * connected/disconnected/errorOccurred signals, and calls connectToDevice().
     */
    void onDeviceDiscovered(const QBluetoothDeviceInfo &device);

    /**
     * @brief Handles the end of the BLE scan.
     *
     * Fires on discovery-agent timeout or after a manual stop() once the
     * target device is found. Currently logs only.
     */
    void onScanFinished();

    /**
     * @brief Handles a successful GATT connection to the ESP32.
     *
     * Updates internal state via setConnected(true).
     */
    void onControllerConnected();

    /**
     * @brief Handles disconnection from the ESP32.
     *
     * Updates internal state via setConnected(false).
     */
    void onControllerDisconnected();

    /**
     * @brief Handles errors reported by the QLowEnergyController.
     * @param error Error code (timeout, host out of range, etc.).
     *
     * Logs the error and marks the connection inactive via setConnected(false).
     * Does not distinguish between error types or attempt a retry.
     */
    void onControllerError(QLowEnergyController::Error error);

    /**
     * @brief Handles GATT service discovery.
     *
     * Called when a BLE service is discovered on the connected device.
     *
     * @param[in] uuid UUID of the discovered service.
     */
    void onServiceDiscovered(const QBluetoothUuid &uuid);

    /**
     * @brief Handles changes to the BLE service state.
     *
     * Called when the state of the discovered BLE service changes.
     *
     * @param[in] state New state of the BLE service.
     */
    void onServiceStateChanged(QLowEnergyService::ServiceState);

    /**
     * @brief Handles changes to a BLE characteristic value.
     *
     * Called when a notification or indication is received from a
     * subscribed BLE characteristic.
     *
     * @param[in] c BLE characteristic that generated the update.
     * @param[in] value New value received from the characteristic.
     */
    void onCharacteristicChanged(const QLowEnergyCharacteristic &c,
                                 const QByteArray &value);

private:
    /**
     * @brief Updates the internal connection state and notifies on change.
     * @param value New connection state.
     *
     * Only emits connectedChanged() if the value actually changed, avoiding
     * redundant signals to QML bindings.
     */
    void setConnected(bool value);

    QBluetoothDeviceDiscoveryAgent* m_discoveryAgent = nullptr; ///< BLE scanning agent.
    QLowEnergyController* m_controller = nullptr;               ///< Central GATT controller to the ESP32.
    bool m_connected = false;                                   ///< Current connection state.

    /**
     * @brief GATT service discovered on the ESP32.
     *
     * Provides access to the characteristics exposed by the thermostat
     * BLE service.
     */
    QLowEnergyService* m_service = nullptr;

    /**
     * @brief Current temperature received from the ESP32.
     *
     * Temperature value in degrees Celsius.
     */
    double m_currentTemp = 0.0;

    /**
     * @brief UUID of the thermostat GATT service.
     *
     * Used to identify the custom thermostat service during BLE service
     * discovery.
     */
    static constexpr auto kThermostatServiceUuid =
        "{59462f12-9543-9999-12c8-58b459a2712d}";
};