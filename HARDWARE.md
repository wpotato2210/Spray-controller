# HARDWARE.md

Spray Controller Hardware

## Hardware Summary

- Single flow sensor for total flow feedback.
- Multiple boom sections controlled independently as ON/OFF outputs.
- 12V pump power stage with 5V MCU logic.

## Pinout (board profiles)

### Arduino Nano (`ARDUINO_AVR_NANO`)

|Pin|Component|Notes|
|---|---|---|
|2|Wheel Sensor|Digital pulse input|
|3|Flow Sensor|Single total-flow pulse input (YF-S201C)|
|11|Pump PWM|PWM output to 12V pump driver|
|12|Boom Section 1|Digital output (ON/OFF)|
|13|Boom Section 2|Digital output (ON/OFF)|
|A1|Boom Section 3|Digital output (ON/OFF)|
|A2|Section 1 LED|Indicator|
|A3|Run/Hold Switch|Input pull-up|
|A4|Section Switch 1|Input pull-up|
|A5|Section Switch 2|Input pull-up|
|4|Section Switch 3|Input pull-up; avoids A6 analog-only behavior|
|A0|Pressure Sensor (Optional)|Analog telemetry input; feature-gated and disabled by default|

### Arduino Uno (`ARDUINO_AVR_UNO`)

Uses the same mapping as Nano for this firmware profile.

### Unsupported boards

Build intentionally fails with `#error` unless `ARDUINO_AVR_NANO` or
`ARDUINO_AVR_UNO` is selected. Add a dedicated `include/pins_<board>.h` mapping
before adding new board targets.

## Switches

- Individual section on/off
- Run/Hold switch with optional bypass solenoid

## Power

- Pump: 12V supply
- MCU: 5V logic

## ASCII Wiring Diagram

```text
MCU Nano (5V logic)
+----------------------+
| 2  -> Wheel Sensor   |----> Pulse input
| 3  -> Flow Sensor    |----> Single total-flow pulse input
| 11 -> Pump PWM       |----> 12V Pump via N-MOSFET / Relay
| 12 -> Boom 1         |----> Solenoid 1 (ON/OFF)
| 13 -> Boom 2         |----> Solenoid 2 (ON/OFF)
| A1 -> Boom 3         |----> Solenoid 3 (ON/OFF)
| A2 -> LED Boom 1     |----> Indicator LED
| A0 -> Pressure In    |----> Optional analog pressure telemetry
| GND -----------------|----> Common GND
+----------------------+
```
