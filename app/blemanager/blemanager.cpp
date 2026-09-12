#include "blemanager.h"
#include <QDebug>

/**
 * @brief Constructs the BleManager and initializes the BLE discovery agent.
 *
 * Creates the QBluetoothDeviceDiscoveryAgent as a child of this object (so it
 * is automatically destroyed along with the BleManager) and connects:
 *  - deviceDiscovered -> onDeviceDiscovered: fired for every BLE device found.
 *  - finished         -> onScanFinished: fired when the scan ends.
 */
BleManager::BleManager(QObject* parent) : QObject(parent) {
    m_discoveryAgent = new QBluetoothDeviceDiscoveryAgent(this);
    connect(m_discoveryAgent, &QBluetoothDeviceDiscoveryAgent::deviceDiscovered,
            this, &BleManager::onDeviceDiscovered);
    connect(m_discoveryAgent, &QBluetoothDeviceDiscoveryAgent::finished,
            this, &BleManager::onScanFinished);
}

/**
 * @brief Starts the BLE scan.
 *
 * Uses LowEnergyMethod to restrict the search to BLE devices only (excludes
 * classic Bluetooth), reducing scan time and noise in the results.
 */
void BleManager::startScan() {
    qDebug() << "Starting BLE scan...";
    m_discoveryAgent->start(QBluetoothDeviceDiscoveryAgent::LowEnergyMethod);
}

/**
 * @brief Slot invoked for each BLE device discovered during scanning.
 * @param device Information about the discovered device (name, address, etc.).
 *
 * Filters by the advertising name "nimble-bleprph" (the ESP32's NimBLE
 * firmware). Once found:
 *  1. Stops the scan to save radio/battery and avoid duplicate reconnections.
 *  2. Creates the central QLowEnergyController associated with that device.
 *  3. Connects the controller's connected/disconnected/errorOccurred signals.
 *  4. Requests the GATT connection via connectToDevice().
 */
void BleManager::onDeviceDiscovered(const QBluetoothDeviceInfo &device) {
    if (device.name() == "nimble-bleprph") {
        qDebug() << "Found ESP32:" << device.name() << device.address();
        m_discoveryAgent->stop();

        m_controller = QLowEnergyController::createCentral(device, this);
        connect(m_controller, &QLowEnergyController::connected,
                this, &BleManager::onControllerConnected);
        connect(m_controller, &QLowEnergyController::disconnected,
                this, &BleManager::onControllerDisconnected);
        connect(m_controller, &QLowEnergyController::errorOccurred,
                this, &BleManager::onControllerError);

        m_controller->connectToDevice();
    }
}

/**
 * @brief Slot invoked when the BLE scan finishes.
 *
 * Fires either due to the discovery agent's timeout or because stop() was
 * called manually after finding the expected device. Currently it only logs
 * a message; it does not distinguish between the two cases or retry the scan
 * if the device wasn't found.
 */
void BleManager::onScanFinished() {
    qDebug() << "Scan finished, device not found (or already connected)";
}

/**
 * @brief Slot invoked when the QLowEnergyController connects successfully.
 *
 * Marks the internal state as connected via setConnected(true), which
 * triggers connectedChanged() if the value actually changed.
 */
void BleManager::onControllerConnected() {
    qDebug() << "Connected to ESP32!";
    setConnected(true);
}

/**
 * @brief Slot invoked when the QLowEnergyController disconnects.
 *
 * Marks the internal state as disconnected via setConnected(false).
 */
void BleManager::onControllerDisconnected() {
    qDebug() << "Disconnected from ESP32";
    setConnected(false);
}

/**
 * @brief Slot invoked on any QLowEnergyController error.
 * @param error Error code reported by Qt Bluetooth.
 *
 * Logs the error and forces setConnected(false), assuming any error implies
 * loss of a usable connection. Does not implement retry logic or distinguish
 * between error types.
 */
void BleManager::onControllerError(QLowEnergyController::Error error) {
    qDebug() << "BLE Controller error:" << error;
    setConnected(false);
}

/**
 * @brief Updates the connection state and notifies listeners (QML) if it changed.
 * @param value New connection value.
 *
 * Avoids emitting connectedChanged() when the value doesn't actually change,
 * following the standard Qt/QML property setter pattern.
 */
void BleManager::setConnected(bool value) {
    if (m_connected != value) {
        m_connected = value;
        emit connectedChanged();
    }
}