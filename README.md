# README

Spray Controller

## Overview

Automated boom sprayer controller for precision agriculture using a global
flow-control model.

## System Model

- Single FlowSensor measures total system flow (L/min)
- Multiple BoomSection valves are binary ON/OFF outputs
- One global FlowController computes pump PWM from target rate, speed, and
  active width

## Core Components

- MCU: Arduino Nano (5V logic)
- Pump: 12V PWM-controlled
- Boom sections: independent binary valve outputs
- Sensors: single YF-S201C flow sensor plus wheel speed sensor
- Operator control: run/hold switch

## OpenSpec Source of Truth

- ARCHITECTURE.md for module interfaces
- HARDWARE.md for pin mappings
- PROTOCOLS.md for message formats
- CONFIGURATION.md for constants and units

## Quickstart

1. Wire hardware according to HARDWARE.md.
2. Configure constants from CONFIGURATION.md.
3. Install and flash firmware from INSTALL.md.
4. Operate with run/hold and section switches from USAGE.md.

## Dependencies

- Arduino IDE
- Required libraries: Servo and Wire

## Placeholders

- High-level project diagram
- Optional MCU alternatives beyond Nano
