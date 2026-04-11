# HARDWARE.md

Spray Controller Hardware

## Hardware Summary

- Single flow sensor for total flow feedback.
- Multiple boom sections controlled independently as ON/OFF outputs.
- 12V pump power stage with 5V MCU logic.
- ST7920 128x64 LCD supported on Arduino Mega in serial/SPI mode via U8g2 (`U8G2_ST7920_128X64_F_SW_SPI`).

## Pinout (board profiles)

### Arduino Mega 2560 (`ARDUINO_AVR_MEGA2560`)

| Pin | Component | Notes |
| --- | --- | --- |
| 18 | Wheel Sensor | Digital pulse input (external interrupt capable) |
| 19 | Flow Sensor | Single total-flow pulse input (YF-S201C, external interrupt capable) |
| 44 | Pump PWM | PWM output to 12V pump driver |
| 22 | Boom Section 1 | Digital output (ON/OFF) |
| 23 | Boom Section 2 | Digital output (ON/OFF) |
| 24 | Boom Section 3 | Digital output (ON/OFF) |
| 25 | Section 1 LED | Indicator |
| 26 | Run/Hold Switch | Input pull-up |
| 27 | Section Switch 1 | Input pull-up |
| 28 | Section Switch 2 | Input pull-up |
| 29 | Section Switch 3 | Input pull-up |
| A8 | Pressure Sensor (Optional) | Analog telemetry input; feature-gated and disabled by default |
| 13 | LCD SCLK / E | ST7920 serial clock (U8g2 SW SPI clock) |
| 11 | LCD MOSI / RW | ST7920 serial data (U8g2 SW SPI data) |
| 10 | LCD CS / RS | ST7920 chip select |
| 8 | LCD RST | ST7920 reset |

#### ST7920 wiring requirements

- Tie **PSB LOW** to force serial/SPI mode.
- Connect **VO** to contrast potentiometer wiper (typical 10k pot between 5V and GND).
- Wire LCD backlight pins through appropriate resistor/driver path for the module variant.
- Keep display mode single-interface only (serial/SPI); do not mix with parallel wiring.

### Arduino Nano (`ARDUINO_AVR_NANO`)

| Pin | Component | Notes |
| --- | --- | --- |
| 2 | Wheel Sensor | Digital pulse input |
| 3 | Flow Sensor | Single total-flow pulse input (YF-S201C) |
| 11 | Pump PWM | PWM output to 12V pump driver |
| 12 | Boom Section 1 | Digital output (ON/OFF) |
| 13 | Boom Section 2 | Digital output (ON/OFF) |
| A1 | Boom Section 3 | Digital output (ON/OFF) |
| A2 | Section 1 LED | Indicator |
| A3 | Run/Hold Switch | Input pull-up |
| A4 | Section Switch 1 | Input pull-up |
| A5 | Section Switch 2 | Input pull-up |
| 4 | Section Switch 3 | Input pull-up; avoids A6 analog-only behavior |
| A0 | Pressure Sensor (Optional) | Analog telemetry input; feature-gated and disabled by default |

### Arduino Uno (`ARDUINO_AVR_UNO`)

Uses the same mapping as Nano for this firmware profile.

## Switches

- Individual section on/off
- Run/Hold switch with optional bypass solenoid

## Power

- Pump: 12V supply
- MCU: 5V logic

## ASCII Wiring Diagram (Mega + ST7920)

```text
MCU Mega2560 (5V logic)
+-----------------------------+
| 18 -> Wheel Sensor          |----> Pulse input (INT)
| 19 -> Flow Sensor           |----> Pulse input (INT)
| 44 -> Pump PWM              |----> 12V Pump via N-MOSFET / Relay
| 22 -> Boom 1                |----> Solenoid 1 (ON/OFF)
| 23 -> Boom 2                |----> Solenoid 2 (ON/OFF)
| 24 -> Boom 3                |----> Solenoid 3 (ON/OFF)
| 26 -> Run/Hold              |----> Toggle input
| 27 -> Section SW 1          |----> Toggle input
| 28 -> Section SW 2          |----> Toggle input
| 29 -> Section SW 3          |----> Toggle input
| 13 -> LCD SCLK (E)          |----> ST7920 serial clock
| 11 -> LCD MOSI (RW)         |----> ST7920 serial data
| 10 -> LCD CS (RS)           |----> ST7920 chip select
|  8 -> LCD RST               |----> ST7920 reset
| PSB -> GND                  |----> Force serial/SPI mode
| A8 -> Pressure In (opt)     |----> Optional analog pressure telemetry
| GND ------------------------|----> Common GND
+-----------------------------+
```
