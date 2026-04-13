# BLE Audio Platform – Engineering Project

## Overview
This repository contains the hardware design, firmware and test results of a custom platform developed for testing and prototyping Bluetooth Low Energy Audio (BLE Audio) systems.

The platform is based on the Nordic nRF5340 SoC and enables testing of LE Audio broadcast transmission using isochronous channels (BIS).

## Repository Structure

- **hardware/** – KiCad project files, schematics and PCB design
- **firmware/** – embedded firmware projects based on nRF Connect SDK
- **docs/** – documentation, images and datasheets
- **test_logs/** – logs from laboratory and real-world BLE Audio transmission tests

## Hardware Overview

The hardware platform includes:
- nRF5340 SoC (via BT40F SiP module)
- PMIC (nPM1304)
- USB–UART interface (CP2105)
- External audio codec (SGTL5000) – not used in final tests due to clock limitations
- BLE antenna with keepout area
- USB-C power input

## Firmware Overview

The firmware is based on the nRF Connect SDK and Zephyr RTOS.

Two main applications are provided:
- **Broadcast Source** – transmits audio streams using BLE Audio BIS
- **Broadcast Sink** – receives BLE Audio streams

Additional test applications are provided for hardware verification (LEDs, I2C, beacon, etc.).

## BLE Audio Testing

Two test scenarios were performed:
1. Laboratory conditions – stable transmission
2. Real-world conditions – interference and obstacles present

Logs from both scenarios are provided in the `test_logs/` directory.

## Known Limitations

- The SGTL5000 audio codec could not be fully used due to insufficient MCLK frequency from the MCU.
- Network core logging is disabled due to memory limitations.
- Audio path testing was therefore limited to LC3 encode/decode and BLE transmission.

## Tools and Technologies

- Nordic nRF Connect SDK
- Zephyr RTOS
- KiCad
- BLE Audio (Bluetooth 5.2)
- LC3 codec

## License

This project is licensed under the terms of the LICENSE file in this repository.
