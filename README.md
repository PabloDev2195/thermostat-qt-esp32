# Thermostat Qt ESP32

A desktop thermostat controller built with **Qt/QML and C++**, communicating with an **ESP32 over Bluetooth Low Energy (BLE)**.

The project combines a modern desktop HMI with embedded firmware, BLE communication, and testable application logic.

## Overview

`thermostat-qt-esp32` is an IoT thermostat prototype designed to demonstrate end-to-end communication between a Qt/QML desktop application and an ESP32 embedded device.

The desktop application provides the user interface and communicates with the ESP32 through BLE to monitor and control temperature-related parameters.

The ESP32 firmware is developed using **ESP-IDF** and **NimBLE**, with a modular architecture separating application logic, hardware abstraction, and communication layers.

## Architecture

```text
┌──────────────────────────────────────┐
│          Qt/QML Desktop App          │
│                                      │
│  HMI / UI                            │
│  C++ Application Logic               │
│  BLE Client                          │
│  Unit Tests / Dependency Injection   │
└──────────────────┬───────────────────┘
                   │
                   │ Bluetooth Low Energy
                   │
┌──────────────────▼───────────────────┐
│              ESP32                   │
│                                      │
│  NimBLE BLE Server                   │
│  Temperature / ADC                   │
│  Embedded Application Logic          │
│  Hardware Interfaces                 │
│                                      │
│              ESP-IDF                 │
└──────────────────────────────────────┘
```

## Features

### Desktop Application

* Qt 6 / QML user interface
* C++ application logic
* BLE communication with ESP32
* Separation between UI and application logic
* Dependency injection for testability
* Unit testing of core application components
* Asynchronous BLE communication

### ESP32 Firmware

* ESP-IDF based firmware
* NimBLE BLE stack
* BLE GATT server
* Custom services and characteristics
* ADC-based temperature acquisition
* Modular component-based architecture
* Callback-based BLE event handling
* Hardware/application layer separation

## Technologies

| Area              | Technology     |
| ----------------- | -------------- |
| Desktop UI        | Qt / QML       |
| Desktop Logic     | C++            |
| Embedded Firmware | C              |
| MCU               | ESP32-WROOM-32 |
| Framework         | ESP-IDF        |
| Bluetooth         | BLE / NimBLE   |
| Testing           | Unit Tests     |
| Build System      | CMake          |
| Version Control   | Git            |

## Project Structure

```text
thermostat-qt-esp32/
│
├── firmware/
│   ├── components/
│   │   ├── adc/
│   │   ├── ble_mgr/
│   │   └── temperature/
│   │
│   └── main/
│
├── desktop/
│   ├── src/
│   ├── qml/
│   ├── tests/
│   └── ...
│
├── CMakeLists.txt
└── README.md
```

> The project structure may evolve as additional hardware interfaces and application features are added.

## BLE Communication

The ESP32 operates as a **BLE GATT server**, exposing services and characteristics used by the Qt desktop application.

The desktop application acts as the **BLE client**, discovering the ESP32 services and interacting with the corresponding characteristics.

The communication flow is approximately:

```text
Qt/QML Application
        │
        ▼
   C++ BLE Client
        │
        │ BLE
        ▼
   ESP32 BLE Server
        │
        ▼
   GATT Services
        │
        ▼
Temperature / Control Data
```

This approach provides a clear separation between the desktop HMI and the embedded device.

## Testing

The application is designed with testability in mind.

Core logic uses **dependency injection** to reduce coupling with hardware-dependent components, allowing functionality to be tested independently.

The goal is to keep application logic testable without requiring a physical ESP32 for every test.

## Getting Started

### Requirements

#### Desktop

* Qt 6
* C++17 or later
* CMake
* Bluetooth Low Energy support

#### Firmware

* ESP-IDF 6.x
* ESP32-WROOM-32
* USB connection for flashing and debugging

### Build Firmware

```bash
cd firmware

idf.py set-target esp32

idf.py build

idf.py flash monitor
```

### Build Desktop Application

Configure the Qt project using CMake and build it with your preferred Qt-supported compiler/toolchain.

## Development Goals

This project is also being used as a practical embedded/software engineering project to explore:

* BLE client/server architecture
* Qt/QML desktop HMI development
* Embedded C with ESP-IDF
* Hardware/software integration
* Dependency injection and testable architecture
* Asynchronous communication
* Embedded-to-desktop protocols
* Modular CMake-based projects
* IoT device integration

## Roadmap

* [x] ESP32 firmware project
* [x] ADC temperature acquisition
* [x] BLE GATT server
* [x] Qt/QML desktop application
* [x] BLE communication layer
* [x] Dependency injection
* [ ] Unit testing
* [ ] Temperature control
* [ ] Improved BLE error handling
* [ ] Connection/reconnection management
* [ ] Additional thermostat controls
* [ ] Hardware prototype integration

## License

This project is intended as a personal engineering and learning project.
