# Multi-Sensor Embedded Station — Arduino Mega / STM32 NUCLEO L476

Embedded multi-sensor acquisition station with real-time display, PWM actuation, and threshold-based alerting. Developed on Arduino Mega, architectured for portability to STM32 NUCLEO L476.

---

## Overview

This project implements a modular embedded station that acquires data from multiple heterogeneous sensors, displays results on an LCD, and triggers actuators based on configurable thresholds. The architecture is designed from the ground up for portability to STM32, with a clear separation between hardware abstraction and application logic.

**Key achievement:** Modular sensor architecture with HAL-oriented design, enabling a structured migration path to STM32 STM32CubeIDE.

---

## Hardware

| Component | Role | Interface |
|---|---|---|
| Arduino Mega 2560 | Main microcontroller (prototype) | — |
| DS18B20 | Temperature sensor | 1-Wire |
| Water level sensor | Analog level detection | ADC |
| Analog joystick | 2-axis user input | ADC (X/Y) |
| LCD 16×2 (I2C) | Real-time data display | I2C |
| Servo motor | PWM actuation (joystick-controlled) | PWM |
| Active buzzer | Threshold alert | GPIO |

---

## Firmware Architecture

Developed in **C/C++**, structured for STM32 portability from the start.

```
Core/
├── App/
│   ├── app.c        # FreeRTOS tasks — temp, water, joystick, display, alert
│   └── app.h        # Task handles, queues, mutex, thresholds
├── Drivers/
│   ├── ds18b20.c / ds18b20.h    # 1-Wire temperature (bit-bang, no lib)
│   ├── sensors.c / sensors.h    # Water level + joystick (ADC)
│   ├── servo.c / servo.h        # Servo motor PWM (TIM3)
│   └── lcd_i2c.c / lcd_i2c.h   # LCD 16x2 I2C (PCF8574, 4-bit mode)
├── main.c           # Entry point — peripheral init, FreeRTOS start
└── main.h           # Pin definitions, HAL handles
```

### Key implementation details

- **DS18B20 (1-Wire)** — bit-bang protocol with precise µs timing, CRC checksum validation, no external library
- **Water level** — ADC acquisition with multi-sample averaging to filter noise, mapped to percentage display
- **Joystick** — dual-axis ADC (X/Y), mapped to servo angle (0°–180°) via linear interpolation
- **LCD 16×2 I2C** — PCF8574 expander, 4-bit mode, structured refresh at fixed rate (no blocking delay in display loop)
- **Buzzer alerting** — threshold-based trigger: temperature > T_max or water level > L_max activates buzzer, logged to serial
- **Servo PWM** — joystick position mapped to PWM duty cycle, smooth angle transitions

---

## Real-Time Behavior

| Task | Period | Trigger |
|---|---|---|
| Temperature acquisition | 1 s | Timer |
| Water level acquisition | 500 ms | Timer |
| Joystick + servo update | 50 ms | Timer |
| LCD refresh | 200 ms | Timer |
| Alert check | 100 ms | Timer |

All timing managed via hardware timers — no `delay()` calls in the main loop.

---

## STM32 Migration — NUCLEO L476

The architecture is designed for direct portability to STM32 HAL:

| Arduino layer | STM32 equivalent |
|---|---|
| `analogRead()` | `HAL_ADC_Start()` / `HAL_ADC_GetValue()` |
| `analogWrite()` | `HAL_TIM_PWM_Start()` |
| `micros()` | TIM counter at 1 MHz |
| `Wire.h` (I2C) | `HAL_I2C_Master_Transmit()` |
| `Serial.print()` | `HAL_UART_Transmit()` |

Migration approach: swap BSP layer only — Drivers/ and App/ remain unchanged.

> Migration to STM32 NUCLEO L476 in progress. STM32CubeIDE project and HAL drivers to be added.

---

## Tools & Environment

| Tool | Usage |
|---|---|
| Arduino IDE | Prototype development |
| STM32CubeIDE | STM32 migration target |
| STM32CubeMX | Peripheral configuration |
| Git | Version control |
| PuTTY | Serial debug monitoring |

---

## Skills Demonstrated

- Multi-sensor embedded firmware in C/C++
- Hardware abstraction layer design (BSP / Drivers / App)
- Protocols : I2C, 1-Wire, ADC, PWM, UART
- Threshold-based real-time alerting
- Architecture oriented toward platform portability (Arduino → STM32)
