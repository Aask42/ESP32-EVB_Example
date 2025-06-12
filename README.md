# ESP32-EVB Example Project

This repository contains example code for the ESP32-EVB development board by Olimex, demonstrating LED control, button handling, relay operation, and ESP-NOW wireless communication.

## Overview

This project showcases various capabilities of the ESP32-EVB board:

- **RGB LED Control**: Controls a WS2812B LED matrix (8x8) with color cycling effects
- **Button Handling**: Detects button presses on the Olimex board
- **Relay Control**: Toggles onboard relays in response to button presses
- **ESP-NOW Communication**: Implements wireless communication between ESP32 devices
- **UART Menu Interface**: Provides a simple menu system for controlling the device via serial connection

## Hardware Requirements

- [Olimex ESP32-EVB](https://www.olimex.com/Products/IoT/ESP32/ESP32-EVB/open-source-hardware) development board
- WS2812B LED matrix (8x8 configuration)
- USB cable for programming and serial communication

## Software Requirements

- [PlatformIO](https://platformio.org/) with ESP-IDF framework
- Required libraries:
  - roboticsbrno/SmartLeds (for LED control)

## Project Structure

- `src/` - Main source code
  - `main.cpp` - Primary application code
  - `led_controller.cpp/h` - LED control functionality
  - `esp_now_broadcast.c/h` - ESP-NOW wireless communication
  - `common.h` - Shared definitions and configurations
  - `shared.cpp` - Shared utility functions
- `include/` - Header files
- `lib/` - Project libraries
- `platformio.ini` - PlatformIO configuration

## Pin Configuration

- **LED Data Pin**: GPIO 2
- **Relay 1**: GPIO 32
- **Relay 2**: GPIO 33
- **Button**: GPIO 34

## Features

### LED Control

The project controls a WS2812B LED matrix with 64 LEDs. The LEDs cycle through different hues automatically, creating a rainbow effect. The brightness can be adjusted through the UART interface.

### Button Functionality

When the onboard button is pressed:
- The relay is activated
- A message is printed to the serial console
- If ESP-NOW is enabled, a message is broadcast to other ESP32 devices

### ESP-NOW Communication

The project implements a simple ESP-NOW broadcast system that allows multiple ESP32 devices to communicate wirelessly. When a button is pressed on one device, it can trigger actions on other devices.

### UART Menu

A simple menu system is provided via the serial interface:
- Option 1: Flash LEDs and toggle relay
- Option 2: Reserved for future use
- Option 3: Reserved for future use
- h: Display help menu

## Getting Started

### Installation

1. Clone this repository
2. Open the project in PlatformIO
3. Build and upload to your ESP32-EVB board

### Serial Monitor

Connect to the serial monitor at 115200 baud to interact with the UART menu.

## Configuration

The project can be configured through:

- `platformio.ini` - Build flags and library dependencies
- `common.h` - LED configuration
- `main.cpp` - Device behavior and task priorities

## License

This project is licensed under the terms specified in the LICENSE file.

## Author

Amelia Wietting