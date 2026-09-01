# Design Decisions — Qt/QML Thermostat + ESP32 (BLE)

This document records the architectural decisions behind this project and the
reasoning that led to them. It is meant to complement the README (which
explains *how to run it*) by explaining *why it's built this way*.

## 1. High-level architecture

```
QML UI  ──►  ThermostatCore (C++, Qt-agnostic)  ──►  ITransport (interface)
                     │                                     │
                     ▼                                     ▼
               Repository (SQLite)              QBluetoothTransport (real)
                                                  MockTransport (tests)
```

`ThermostatCore` contains all business logic (target temperature, heater
state, validation) and has no dependency on QML, `QBluetoothLE`, or SQLite
directly. It depends only on the `ITransport` and `IRepository` interfaces.
This keeps the core logic unit-testable without a running Qt event loop, a
real ESP32, or a database.

## 2. `ITransport` contract

```cpp
class ITransport {
public:
    virtual ~ITransport() = default;

    virtual void connectToDevice() = 0;
    virtual void disconnectFromDevice() = 0;
    virtual bool isConnected() const = 0;

    virtual void sendTargetTemperature(float celsius) = 0;

    // Callback-based, so ThermostatCore doesn't depend on Qt signals/slots.
    virtual void onDataReceived(
        std::function<void(float currentTemp, float targetTemp, bool heating)> callback) = 0;
    virtual void onConnectionStateChanged(
        std::function<void(bool connected)> callback) = 0;
};
```

**Why an interface instead of a concrete class:** it allows two
implementations — `QBluetoothTransport` (real BLE) and `MockTransport`
(deterministic, in-memory, used in unit tests) — to be swapped without
touching `ThermostatCore`. This is also what allowed the transport mechanism
to change mid-project (see Section 6) without any redesign of the core logic.

## 3. BLE protocol (GATT)

**Service:** `Thermostat Service`
UUID: `7f04c1c5-79a6-422f-b2e7-41a865125201`

| Characteristic | UUID | Type | Properties | Notes |
|---|---|---|---|---|
| CurrentTemp | `f8bd961c-d2cb-4449-8de7-289598e788e6` | `float32` (4 bytes, little-endian) | Read, Notify | Pushed by ESP32 whenever the sensor reading changes by more than 0.1°C |
| TargetTemp | `4b9a84ef-fa7d-414b-8a5b-8fd7f23ba92f` | `float32` (4 bytes, little-endian) | Read, Write | Written by the app when the user changes the setpoint |
| HeaterState | `7527687c-e418-4e3c-8c35-5aca4b27d1c1` | `uint8` (0 = off, 1 = heating) | Read, Notify | Reflects the actual relay/PWM state on the ESP32 |

Payload example (CurrentTemp = 21.8°C):
```
bytes: [0x66, 0x66, 0xAE, 0x41]   // IEEE-754 float32 LE for 21.8
```

**Why typed characteristics instead of a single text stream:** each value
arrives already typed and doesn't require a line-based parser or framing
logic, which removes a whole class of bugs (partial reads, delimiter
corruption) that a text protocol over a stream would need to handle.

## 4. Database schema (SQLite)

```sql
CREATE TABLE sessions (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    started_at TEXT NOT NULL,   -- ISO 8601
    ended_at   TEXT             -- NULL while session is active
);

CREATE TABLE readings (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    session_id INTEGER NOT NULL REFERENCES sessions(id),
    timestamp TEXT NOT NULL,    -- ISO 8601
    current_temp REAL NOT NULL,
    target_temp REAL NOT NULL,
    heating INTEGER NOT NULL    -- 0/1
);

CREATE TABLE setpoint_changes (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    session_id INTEGER NOT NULL REFERENCES sessions(id),
    timestamp TEXT NOT NULL,
    previous_target REAL NOT NULL,
    new_target REAL NOT NULL
);
```

**Why three tables instead of one flat log:** `sessions` groups readings by
each time the app runs and connects to the device, which makes it possible
to query "history for today's session" without filtering on gaps.
`setpoint_changes` is tracked separately from `readings` because it's a
distinct event (a user action), not a periodic sample — mixing the two would
make both harder to query cleanly.

## 5. Testing strategy

| Component | Tested with | Requires hardware? |
|---|---|---|
| `ThermostatCore` | QTest + `MockTransport` | No |
| Protocol payload encode/decode | QTest | No |
| `Repository` (SQL) | QTest + in-memory SQLite (`:memory:`) | No |
| `QBluetoothTransport` | Manual testing only | Yes (real ESP32) |

Everything except the BLE transport implementation itself is covered by
automated tests. The BLE transport is intentionally left to manual/
integration testing, since mocking `QLowEnergyController` would test Qt's
BLE stack more than our own code.

## 6. Decisions considered and discarded

- **Serial/UART instead of wireless:** simpler and was the original plan, but
  discarded in favor of Bluetooth to demonstrate wireless IoT communication
  skills as well.
- **Bluetooth Classic (SPP) instead of BLE:** SPP would have let the existing
  text-based protocol design be reused almost as-is (it behaves like a
  virtual serial port). BLE was chosen instead because it is the modern,
  better-supported standard on both the Qt (`QLowEnergyController`) and ESP32
  (NimBLE) sides, and better demonstrates familiarity with GATT-based
  communication.
- **CAN bus instead of Bluetooth:** considered and discarded — CAN is
  designed for multi-node buses with message prioritization (e.g.,
  automotive), which brings no benefit for a single point-to-point
  connection between one PC and one ESP32.

## 7. Known limitations / out of scope

- No support for multiple ESP32 devices on the same app instance.
- No BLE pairing/authentication — assumes a trusted, local environment.
- No PID control loop; the thermostat only supports manual on/off heating
  based on target vs. current temperature.