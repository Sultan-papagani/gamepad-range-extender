# XInput to BLE Gamepad Bridge

## Description

A custom hardware and software bridge that converts proprietary 2.4GHz XInput wireless gamepads (e.g., GameSir Nova 2 Lite) into universal Bluetooth Low Energy (BLE) gamepads. The system utilizes a Raspberry Pi Pico to bypass Microsoft's proprietary Xbox 360 wireless protocol and a Realtek BW-16 to remap and broadcast the controller state as a standard HID device over BLE.

<img width="1280" height="720" alt="tutorial" src="https://github.com/user-attachments/assets/1e63dead-2ba8-4899-8839-63db84c9a7b3" />

<img width="1280" height="720" alt="tutoorial2" src="https://github.com/user-attachments/assets/69f4b9bd-83d7-4247-84dc-3be8c190f3ac" />


## Architecture

The project operates in two distinct phases distributed across two microcontrollers:

1. **Raspberry Pi Pico**: Configured as a dedicated USB Host. It mounts the 2.4GHz dongle using a microusb OTG cable.
2. **Ai-Thinker BW-16**: Receives the UART stream, and translates the XInput into standard USB HID gamepad format, broadcast the controller state as a standard Gamepad HID device over BLE

## Hardware Requirements

* Raspberry Pi Pico (RP2040)
* Realtek Ameba BW-16 (RTL8720DN)
* USB OTG Cable (or direct USB breakout to Pico test pads)
* 2.4GHz XInput Gamepad Dongle

## Wiring Diagram

Power is supplied to the BW-16, which passes 5V directly to the Pico's VBUS pin to power both the Pico and the attached USB dongle.

| BW-16 Pin | Pi Pico Pin | Function |
| --- | --- | --- |
| `5V` | `VBUS` (Pin 40) | Power supply to Pico and USB Host |
| `GND` | `GND` (Pin 38) | Common Ground |
| `PB2` (RX) | `GP0` (TX) | Serial Data (Pico to BW-16) |
| `PB1` (TX) | `GP1` (RX) | Serial Data (BW-16 to Pico) - Optional |

## Software Setup

### 1. Raspberry Pi Pico (XInput Sniffer)

1. Download the provided pi pico sketch.
2. Set **Tools > USB Stack** to "Adafruit TinyUSB Host (native)".
3. Flash the Pico.

### 2. Realtek BW-16 (BLE Broadcaster)

The BW-16 utilizes the standard Ameba BLE libraries.

1. Ensure the Ameba RTL8722DM/RTL8720DN board package is installed.
2. Flash the provided BW-16 sketch.
3. The board will boot and advertise as `GameSir Bridge`.

## Payload Structure

The UART communication uses a strict 15-byte packet to ensure data integrity at high baud rates.

* `Byte 0:` Sync Header (0xAA)
* `Byte 1:` Sync Header (0xBB)
* `Byte 2:` Payload Length (0x0C / 12 bytes)
* `Bytes 3-14:` Raw XInput struct (wButtons, bLeftTrigger, bRightTrigger, sThumbLX, sThumbLY, sThumbRX, sThumbRY)

## Notes

* currently it only works about a minute then crashes, i will fix it later.
* you can edit the code to make it work with keyboard and mouse, and it is easier than this xinput gamepad stuff, just ask an llm to do it
