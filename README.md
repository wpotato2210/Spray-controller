# Spray Controller

## Overview

Automated boom sprayer controller for precision agricultural application using
one global flow-control model.

## System Model (authoritative summary)

- Single FlowSensor measures total system flow (L/min).
- Multiple BoomSection valves are binary ON/OFF outputs.
- One global FlowController computes one pump PWM command from target rate,

speed, and active width.

## Core Components

- MCU: Arduino Nano (ATmega328P, 5V logic).
- Pump: 12V PWM-controlled.
- Boom sections: independent binary valve outputs.
- Sensors: single YF-S201C flow sensor plus wheel speed sensor.
- Operator control: run/hold switch.

## OpenSpec Source of Truth

- `ARCHITECTURE.md`: module interfaces.
- `HARDWARE.md`: pin mappings.
- `WIRING.md`: wiring reference and connection map.
- `PROTOCOLS.md`: message formats.
- `CONFIGURATION.md`: constants and units.
- `DEVELOPMENT_ROADMAP.md`: phased implementation plan.

## Quickstart

1. Wire hardware according to `HARDWARE.md`.
1. Configure constants from `CONFIGURATION.md`.
1. Install and flash firmware (see `INSTALL.md`).
1. Operate with run/hold and section switches (see `USAGE.md`).

## Dependencies

- Arduino IDE.
- Required libraries:
  - [`U8g2`](https://github.com/olikraus/u8g2) (install from Arduino Library
    Manager; current validated baseline: `v2.35.x`).
  - Arduino core (`Arduino.h`).

## AVR memory note

- AVR builds now use the U8g2 ST7920 page-buffer backend (`_1_`) to reduce SRAM
  pressure compared with the full-frame (`_F_`) buffer mode.

## Board support and compile guard

- Supported compile targets: `ARDUINO_AVR_NANO`, `ARDUINO_AVR_UNO`.
- Firmware fails at compile time with a clear `#error` on unsupported board

targets.

## Placeholders

- High-level project diagram.
- Supported MCU alternatives beyond Nano (optional).
