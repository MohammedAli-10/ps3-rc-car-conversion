# Wireless-PS3-Controlled-RC-Car-Conversion
Embedded wireless RC car conversion with PS3 DualShock controller, ESP32, ATmega32, custom EasyEDA PCB, audio and haptic feedback

## Project Overview

A salvaged toy RC car converted into a wireless, feature-rich mechatronic vehicle controlled over Bluetooth using a PlayStation 3 (PS3) DualShock controller. The architecture pairs a 30-pin ESP32 module for wireless Bluetooth communication and dynamic rumble feedback with an ATmega32 microcontroller for real-time motor control, servo steering, 4-speed gear limits, collision sensing, lighting, and MP3 sound effects on a custom single-layer EasyEDA PCB fabricated via home toner transfer.

## Main Features

- Wireless PS3 DualShock control via Bluetooth (`Ps3Controller.h`)
- Dual-MCU architecture linked over 9600 bps UART (`Serial2` on ESP32 `RX=GPIO16`, `TX=GPIO17` to ATmega32)
- Progressive throttle and reverse control with deadzone filtering and 4-speed gear limits
- Active motor braking and front wheel servo steering
- Real-time collision/vibration detection with haptic rumble feedback
- MP3 sound effects and toggleable front headlights

## Driving, Controls & Security/Feedback System

- **Throttle & Steering:**
  - `R2` accelerates forward and `L2` accelerates in reverse with deadzone filtering and differential calculation (`r2 - l2` or `l2 - r2`), capped by the active gear's upper limit
  - `LX` controls front wheel steering angle via Timer1 PWM (`PD5`, ~1000 µs to 2000 µs pulse width with center deadzone around 128)
  - `Circle` engages active braking (`Motor_Brake`)
- **4-Speed Gear System (`L1` / `R1`):**
  - Gear 1: Speed limit up to `64` (Player LED 1)
  - Gear 2: Speed limit up to `128` (Player LED 2)
  - Gear 3: Speed limit up to `191` (Player LED 3)
  - Gear 4: Speed limit up to `255` (Player LED 4)
  - Shifting gears updates the speed ceiling on ATmega32, plays the gear shift sound, and sends packet `0x05` to update the PS3 Player LED indicator
- **Haptic Rumble & Collision Feedback:**
  - Periodic speed/rumble updates (`0x06`) sent via Timer0 overflow ticks modulate PS3 controller vibration based on `L2`/`R2` and speed
  - `SW-520D` collision/vibration sensor (soldered directly to `PA1`) detects impacts and sends packet `0x07` to trigger a `300 ms` high-intensity rumble pulse

## Audio, Lighting & Power System

- **DFPlayer Mini + 0.5W 8Ω Speaker (via bit-banged software UART on `PB0`):**
  - `Start` toggles between standby/idle and active driving mode and plays Engine Start sound (Track 1)
  - `Triangle` plays Car Horn sound (Track 2)
  - `L1` / `R1` plays Gear Shift sound (Track 3) after initial gear setup
- **Front Headlights:**
  - 2 white LEDs soldered directly to `PA0`, toggled on/off (`PORTA ^= (1 << PA0)`) using the `Cross` button
- **Power Subsystem:**
  - 3S Lithium battery pack (~12V nominal) feeding the L298 motor driver and an LM2596 step-down buck converter for regulated logic and servo rails
  - DC female charging socket and master ground-path switch

## Hardware, Firmware & Media

- **Custom PCB & Toolchain:**
  - EasyEDA for schematic capture and single-layer PCB layout (fabricated at home via toner transfer, etching, drilling, and soldering)
  - Atmel Studio + USBasp / eXtreme Burner for ATmega32 Embedded C firmware (`USART_RXC_vect`, Timer1 PWM on `PD5`, Timer2 PWM on `PD7`, direction/braking on `PC0`/`PC1`)
  - Arduino IDE (`Ps3Controller.h`) for ESP32 (30-pin) firmware
- **Media & Videos:**
  - Before & after photos, custom PCB assembly photos, and modification/testing photos organized under `media/`
  - Demo and testing videos linked below

## Media Gallery

### Before & After
![Before Conversion](media/before-after/before.jpg)
![After Conversion](media/before-after/after.jpg)

### Custom PCB & Assembly
![Custom PCB](media/pcb/pcb-populated.jpg)

### Modification & Testing
![Modification and Testing](media/build-and-testing/testing-setup.jpg)

## Demo Videos

- [PS3 Driving & Control Demo](https://youtu.be/REPLACE_WITH_VIDEO_1_ID)
- [PCB, Wiring & Bench Testing](https://youtu.be/REPLACE_WITH_VIDEO_2_ID)

## Project Documentation

Complete project documentation including system architecture, hardware components, driving mechanics, and fabrication details can be found below:

📄 [Download Full Documentation (PDF)](docs/Car.pdf)
