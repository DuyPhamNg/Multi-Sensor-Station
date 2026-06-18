# Multi-Sensor-Station
STM32 MultiStation — DS18B20, water level, joystick, LCD I2C, servo PWM, buzzer, FreeRTOS —

Multi-Sensor Embedded Station — Arduino Mega / STM32 NUCLEO L476

Embedded multi-sensor acquisition station with real-time display, PWM actuation, and threshold-based alerting. Developed on Arduino Mega, architectured for portability to STM32 NUCLEO L476.


Overview

This project implements a modular embedded station that acquires data from multiple heterogeneous sensors, displays results on an LCD, and triggers actuators based on configurable thresholds. The architecture is designed from the ground up for portability to STM32, with a clear separation between hardware abstraction and application logic.

Key achievement: Modular sensor architecture with HAL-oriented design, enabling a structured migration path to STM32 STM32CubeIDE.


Hardware

ComponentRoleInterfaceArduino Mega 2560Main microcontroller (prototype)—DS18B20Temperature sensor1-WireWater level sensorAnalog level detectionADCAnalog joystick2-axis user inputADC (X/Y)LCD 16×2 (I2C)Real-time data displayI2CServo motorPWM actuation (joystick-controlled)PWMActive buzzerThreshold alertGPIO


Firmware Architecture

Developed in C/C++, structured for STM32 portability from the start.

project/
├── BSP/              # Hardware abstraction layer
│   ├── bsp_adc.c     # ADC — water level, joystick
│   ├── bsp_pwm.c     # PWM — servo motor
│   └── bsp_gpio.c    # GPIO — buzzer, triggers
├── Drivers/          # Sensor drivers (portable, no Arduino-specific lib)
│   ├── ds18b20.c / ds18b20.h     # 1-Wire temperature
│   ├── water_level.c             # Analog level acquisition
│   ├── joystick.c                # Dual-axis ADC input
│   └── lcd_i2c.c / lcd_i2c.h    # I2C LCD display
└── App/              # Application logic
    ├── display_manager.c         # LCD refresh logic
    ├── alert_manager.c           # Threshold detection + buzzer
    └── servo_control.c           # Joystick → PWM mapping

Key implementation details


DS18B20 (1-Wire) — bit-bang protocol with precise µs timing, CRC checksum validation, no external library
Water level — ADC acquisition with multi-sample averaging to filter noise, mapped to percentage display
Joystick — dual-axis ADC (X/Y), mapped to servo angle (0°–180°) via linear interpolation
LCD 16×2 I2C — PCF8574 expander, 4-bit mode, structured refresh at fixed rate (no blocking delay in display loop)
Buzzer alerting — threshold-based trigger: temperature > T_max or water level > L_max activates buzzer, logged to serial
Servo PWM — joystick position mapped to PWM duty cycle, smooth angle transitions



Real-Time Behavior

TaskPeriodTriggerTemperature acquisition1 sTimerWater level acquisition500 msTimerJoystick + servo update50 msTimerLCD refresh200 msTimerAlert check100 msTimer

All timing managed via hardware timers — no delay() calls in the main loop.


STM32 Migration — NUCLEO L476

The architecture is designed for direct portability to STM32 HAL:

Arduino layerSTM32 equivalentanalogRead()HAL_ADC_Start() / HAL_ADC_GetValue()analogWrite()HAL_TIM_PWM_Start()micros()TIM counter at 1 MHzWire.h (I2C)HAL_I2C_Master_Transmit()Serial.print()HAL_UART_Transmit()

Migration approach: swap BSP layer only — Drivers/ and App/ remain unchanged.


Migration to STM32 NUCLEO L476 in progress. STM32CubeIDE project and HAL drivers to be added.




Tools & Environment

ToolUsageArduino IDEPrototype developmentSTM32CubeIDESTM32 migration targetSTM32CubeMXPeripheral configurationGitVersion controlPuTTYSerial debug monitoring


Skills Demonstrated


Multi-sensor embedded firmware in C/C++
Hardware abstraction layer design (BSP / Drivers / App)
Protocols : I2C, 1-Wire, ADC, PWM, UART
Threshold-based real-time alerting
Architecture oriented toward platform portability (Arduino → STM32)
