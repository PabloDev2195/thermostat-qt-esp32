#pragma once
#include <QObject>
#include <QBluetoothDeviceDiscoveryAgent>
#include <QLowEnergyController>

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

    /// Whether a BLE connection to the ESP32 is currently active.
    Q_PROPERTY(bool connected READ connected NOTIFY connectedChanged)

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

signals:
    /// Emitted whenever the `connected` property value changes.
    void connectedChanged();

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
};