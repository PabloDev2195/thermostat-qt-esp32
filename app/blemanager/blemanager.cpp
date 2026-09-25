#include "blemanager.h"
#include <QDebug>


/**
 * @brief UUID of the BLE temperature characteristic.
 *
 * Identifies the GATT characteristic used to send temperature
 * values on the ESP32.
 */
const QBluetoothUuid BleManager::kTemperatureUuid(
    QStringLiteral("0e0c0b0a-0908-0706-0504-030201efcdab"));
/**
 * @brief UUID of the BLE fan level characteristic.
 *
 * Identifies the GATT characteristic used to control the fan
 * operating level on the ESP32.
 */
const QBluetoothUuid BleManager::kFanLevelUuid(
    QStringLiteral("0f0c0b0a-0908-0706-0504-030201efcdab"));

/**
 * @brief UUID of the BLE setpoint characteristic.
 *
 * Identifies the GATT characteristic used to control the temperature
 * regulation on the ESP32.
 */
const QBluetoothUuid BleManager::kSetpointUuid(
    QStringLiteral("0d0c0b0a-0908-0706-0504-030201efcdab"));

/**
 * @brief UUID of the mode characteristic.
 *
 * Identifies the GATT characteristic used to control the mode
 * operation on the ESP32.
 */
const QBluetoothUuid BleManager::kModeUuid(
    QStringLiteral("0c0c0b0a-0908-0706-0504-030201efcdab"));


/**
 * @brief UUID of the state characteristic.
 *
 * Identifies the GATT characteristic used to send the state
 * operation on the ESP32.
 */
const QBluetoothUuid BleManager::kStateUuid(
    QStringLiteral("0b0c0b0a-0908-0706-0504-030201efcdab"));

/**
 * @brief Constructs the BleManager and initializes the BLE discovery agent.
 *
 * Creates the QBluetoothDeviceDiscoveryAgent as a child of this object (so it
 * is automatically destroyed along with the BleManager) and connects:
 *  - deviceDiscovered -> onDeviceDiscovered: fired for every BLE device found.
 *  - finished         -> onScanFinished: fired when the scan ends.
 */
BleManager::BleManager(QObject* parent) : QObject(parent)
{
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
void BleManager::startScan()
{
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
void BleManager::onScanFinished()
{
    qDebug() << "Scan finished, device not found (or already connected)";
}

/**
 * @brief Slot invoked when the QLowEnergyController connects successfully.
 *
 * Marks the internal state as connected via setConnected(true), which
 * triggers connectedChanged() if the value actually changed.
 */
void BleManager::onControllerConnected()
{
    qDebug() << "Connected to ESP32!";
    setConnected(true);

    connect(m_controller, &QLowEnergyController::serviceDiscovered,
            this, &BleManager::onServiceDiscovered);
    m_controller->discoverServices();
}

/**
 * @brief Slot invoked when the QLowEnergyController disconnects.
 *
 * Marks the internal state as disconnected via setConnected(false).
 */
void BleManager::onControllerDisconnected()
{
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
void BleManager::onControllerError(QLowEnergyController::Error error)
{
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
void BleManager::setConnected(bool value)
{
    if (m_connected != value) {
        m_connected = value;
        emit connectedChanged();
    }
}

/**
 * @brief Handles discovery of BLE GATT services.
 *
 * Called when a remote GATT service is discovered by the BLE controller.
 * Checks whether the discovered service matches the thermostat service UUID.
 *
 * If the thermostat service is found, creates a QLowEnergyService object,
 * connects its state change signal, and starts discovery of its
 * characteristics and descriptors.
 *
 * @param[in] uuid UUID of the discovered GATT service.
 */
void BleManager::onServiceDiscovered(const QBluetoothUuid &uuid)
{
    qDebug() << "Service discovered:" << uuid.toString();

    if (uuid == QBluetoothUuid(QString(kThermostatServiceUuid)))
    {
        qDebug() << "Found thermostat service!";
        m_service = m_controller->createServiceObject(uuid, this);
        if (m_service)
        {
            connect(m_service, &QLowEnergyService::stateChanged,
                    this, &BleManager::onServiceStateChanged);
            m_service->discoverDetails();
        }
    }
}

/**
 * @brief Handles changes to the remote BLE service state.
 *
 * When the thermostat service details have been discovered, iterates
 * through the available characteristics and searches for the temperature
 * characteristic.
 *
 * If the temperature characteristic supports the Client Characteristic
 * Configuration descriptor (CCCD), enables notifications by writing the
 * notification configuration value.
 *
 * @param[in] state Current state of the remote BLE service.
 */
void BleManager::onServiceStateChanged(QLowEnergyService::ServiceState state)
{
    qDebug() << "Service state changed:" << state;

    if (state == QLowEnergyService::RemoteServiceDiscovered)
    {
        qDebug() << "Service details discovered, characteristics:";
        const auto chars = m_service->characteristics();
        for (const auto &ch : chars)
        {
            qDebug() << " -" << ch.uuid().toString();

            if (ch.uuid() == kTemperatureUuid)
            {
                qDebug() << "Found temperature characteristic, enabling notifications...";

                auto cccd = ch.descriptor(QBluetoothUuid::DescriptorType::ClientCharacteristicConfiguration);
                if (cccd.isValid())
                {
                    connect(m_service, &QLowEnergyService::characteristicChanged,
                            this, &BleManager::onTemperatureCharacteristicChanged);
                    m_service->writeDescriptor(cccd, QByteArray::fromHex("0100"));
                }
            }
            if (ch.uuid() == kFanLevelUuid)
            {
                qDebug() << "Found fan level characteristic!";
                m_fanLevelCharacteristic = ch;
            }
            if (ch.uuid() == kSetpointUuid)
            {
                qDebug() << "Found setpoint characteristic!";
                m_setpointCharacteristic = ch;
            }
            if (ch.uuid() == kModeUuid)
            {
                qDebug() << "Found Mode characteristic!";
                m_modeCharacteristic = ch;
            }
            if (ch.uuid() == kStateUuid)
            {
                qDebug() << "Found state characteristic, enabling notifications...";

                auto cccd = ch.descriptor(QBluetoothUuid::DescriptorType::ClientCharacteristicConfiguration);
                if (cccd.isValid())
                {
                    connect(m_service, &QLowEnergyService::characteristicChanged,
                            this, &BleManager::onStateCharacteristicChanged);
                    m_service->writeDescriptor(cccd, QByteArray::fromHex("0100"));
                }
            }
        }
    }
}

/**
 * @brief Handles BLE characteristic value notifications.
 *
 * Processes temperature data received from the ESP32 through the
 * temperature characteristic.
 *
 * The received value is expected to contain a signed 16-bit integer
 * representing the temperature in tenths of a degree Celsius.
 * The value is converted to degrees Celsius and stored in the
 * current temperature property.
 *
 * If the received temperature is different from the current value,
 * emits currentTempChanged() to notify QML of the update.
 *
 * @param[in] c BLE characteristic that generated the notification.
 * @param[in] value Raw characteristic value received from the ESP32.
 */
void BleManager::onTemperatureCharacteristicChanged(
const QLowEnergyCharacteristic &c,
    const QByteArray &value)
{
    if (value.size() >= 2
    && (c.uuid() == kTemperatureUuid))
    {
        int16_t temp_x10;

        memcpy(&temp_x10, value.constData(), sizeof(temp_x10));

        double newTemperature = temp_x10 / 10.0;

        if (m_currentTemp != newTemperature)
        {
            m_currentTemp = newTemperature;
            emit currentTempChanged();
        }
    }
}

/**
  * @brief Handles state characteristic notifications received from the BLE device.
  *
  * Validates the received characteristic and updates the current thermostat
  * state when a valid state value is received.
  *
  * The received state is encoded as a single byte:
  * - 0: Idle
  * - 1: Heating
  * - 2: Cooling
  *
  * The @c currentStateChanged signal is emitted only when the received state
  * differs from the current state.
  *
  * @param[in] c BLE characteristic that generated the notification.
  * @param[in] value Data received from the BLE device.
  */
void BleManager::onStateCharacteristicChanged(
    const QLowEnergyCharacteristic &c,
    const QByteArray &value)
{
    if (value.size() >= 1
    && (c.uuid() == kStateUuid))
    {
        const uint8_t state =
            static_cast<uint8_t>(value.at(0));

        qDebug() << "STATE VALUE =" << state;

        if (m_currentState != state)
        {
            m_currentState = state;
            emit currentStateChanged();
        }
    }
}

/**
 * @brief Sends the requested fan level to the ESP32 via BLE.
 *
 * Checks that the BLE service and fan level characteristic are
 * available before writing the requested level with response.
 *
 * @param level Fan operating level (1 = Low, 2 = Medium, 3 = High).
 *
 * @note The level is transmitted as a single byte.
 * @note If the BLE service or characteristic is unavailable,
 *       the command is not sent.
 */
void BleManager::setFanLevel(quint8 level)
{
    if (m_service != nullptr
        && m_fanLevelCharacteristic.isValid())
    {
        QByteArray data;
        data.append(static_cast<char>(level));

        m_service->writeCharacteristic(
            m_fanLevelCharacteristic,
            data,
            QLowEnergyService::WriteWithResponse);
    }
}


/**
 * @brief Sends the requested temperature setpoint to the ESP32 via BLE.
 *
 * The setpoint is transmitted as an unsigned 16-bit integer with a
 * resolution of 0.1 °C.
 *
 * For example:
 * - 220 -> 22.0 °C
 * - 225 -> 22.5 °C
 * - 250 -> 25.0 °C
 *
 * @param[in] setpoint Temperature setpoint in tenths of a degree Celsius.
 *
 * @note The value is transmitted as two bytes in little-endian order.
 * @note The BLE service and characteristic must be available before
 *       the command can be sent.
 */
void BleManager::setSetpoint(quint16 setpoint)
{
    if (m_service != nullptr
        && m_setpointCharacteristic.isValid())
    {
        QByteArray data;
        data.append(static_cast<char>(setpoint & 0xFF));
        data.append(static_cast<char>((setpoint >> 8) & 0xFF));

        m_service->writeCharacteristic(
            m_setpointCharacteristic,
            data,
            QLowEnergyService::WriteWithResponse);
    }
}

/**
 * @brief Sends the requested control mode to the ESP32 via BLE.
 *
 * Transmits the specified operating mode as a single-byte value
 * through the BLE mode characteristic using a write-with-response
 * operation.
 *
 * @param[in] mode Control operating mode corresponding to a valid
 *                 control_mode_t value defined in the ESP32 firmware.
 *
 * @note The BLE service and mode characteristic must be available
 *       before the command can be sent.
 * @note The mode value is transmitted as a single byte.
 */
void BleManager::setMode(quint8 mode)
{
    if (m_service != nullptr
        && m_modeCharacteristic.isValid())
    {
        QByteArray data;
        data.append(static_cast<char>(mode));

        m_service->writeCharacteristic(
            m_modeCharacteristic,
            data,
            QLowEnergyService::WriteWithResponse);
    }
}