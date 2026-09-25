# ESP32 + Qt/QML BLE Thermostat

A complete embedded thermostat system built with an **ESP32 firmware** and a **Qt/QML desktop application**, communicating through **Bluetooth Low Energy (BLE)**.

The project demonstrates an end-to-end embedded software architecture, from sensor acquisition and actuator control on the microcontroller to BLE communication and a modern desktop HMI.

---

## Overview

The system consists of two main components:

```text
┌──────────────────────────────┐
│          ESP32               │
│                              │
│  C / ESP-IDF / FreeRTOS      │
│                              │
│  ┌────────┐    ┌──────────┐  │
│  │  ADC   │    │   PWM    │  │
│  │ Sensor │    │   Fan    │  │
│  └────┬───┘    └────▲─────┘  │
│       │             │        │
│       └──────┬──────┘        │
│              │               │
│         BLE / NimBLE         │
└──────────────┼───────────────┘
               │
               │ Bluetooth Low Energy
               │
┌──────────────▼───────────────┐
│       Qt/QML Desktop         │
│                              │
│  C++ / Qt 6 / QML            │
│                              │
│  ┌────────────────────────┐  │
│  │      BleManager        │  │
│  │                        │  │
│  │  BLE communication     │  │
│  │  State management      │  │
│  │  QML properties        │  │
│  └───────────┬────────────┘  │
│              │               │
│              ▼               │
│         QML HMI              │
└──────────────────────────────┘
```

---

## Features

### Thermostat

* 🌡️ Real-time temperature monitoring
* 🎯 Temperature setpoint control
* ⏸️ Idle state
* 🔥 Heating state
* ❄️ Cooling state
* 🌀 Three fan speed levels
* 🔵 BLE connection status

### Temperature Processing

The ESP32 samples the temperature periodically and applies a **3-sample averaging filter** before updating the current temperature.

```text
Sensor
  │
  ├── Sample 1 ─┐
  ├── Sample 2 ─┼──> Average ──> Current Temperature
  └── Sample 3 ─┘
```

This helps reduce measurement noise and provides more stable temperature updates.

---

## Technology Stack

### Embedded Firmware

* **ESP32-WROOM-32**
* **ESP-IDF 6.1**
* **C**
* **FreeRTOS**
* **NimBLE**
* ADC
* GPIO
* PWM / LEDC
* BLE GATT
* BLE Notifications

### Desktop Application

* **C++**
* **Qt 6.11**
* **QML**
* **Qt Bluetooth / Low Energy**
* Signals & Slots
* `Q_PROPERTY`
* C++ / QML integration
