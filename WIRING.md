# WIRING.md

Spray Controller Wiring Reference

## Controller pin map (Nano/Uno)

| MCU Pin | Signal | Connect To | Type |
|---|---|---|---|
| D2 | `wheel_sensor` | Wheel speed sensor output | Digital input (pulse) |
| D3 | `flow_sensor` | Flow sensor output | Digital input (pulse) |
| D11 | `pump_pwm` | Pump driver control input | PWM output |
| D12 | `section_1` | Section 1 solenoid driver input | Digital output |
| D13 | `section_2` | Section 2 solenoid driver input | Digital output |
| A1 | `section_3` | Section 3 solenoid driver input | Digital output |
| A2 | `section_1_led` | Section 1 indicator LED (through resistor) | Digital output |
| A3 | `run_hold` | Run/Hold switch | Digital input (pull-up) |
| A4 | `section_switch_1` | Section switch 1 | Digital input (pull-up) |
| A5 | `section_switch_2` | Section switch 2 | Digital input (pull-up) |
| D4 | `section_switch_3` | Section switch 3 | Digital input (pull-up) |
| A0 | `pressure_sensor` (optional) | Pressure sensor analog output | Analog input |

## Power wiring

- 12V rail: pump driver + section valve power stage.
- 5V rail: MCU and logic-level sensor supply.
- Ground: all sensors, drivers, and MCU must share common GND.

## Recommended connection layout

```text
+------------------------+        +--------------------+
| Arduino Nano / Uno     |        | Power + Actuation  |
|                        |        |                    |
| D11 ----------------------->    | Pump driver input  |
| D12 ----------------------->    | Valve driver S1    |
| D13 ----------------------->    | Valve driver S2    |
| A1  ----------------------->    | Valve driver S3    |
| A2  ----------------------->    | Section LED        |
|                        |        |                    |
| D3  <-----------------------    | Flow sensor output |
| D2  <-----------------------    | Wheel sensor out   |
| A0  <-----------------------    | Pressure signal    |
| A3/A4/A5/D4 <---------------    | User switches      |
|                        |        |                    |
| GND ----------------------->    | Common ground      |
+------------------------+        +--------------------+
```

## Notes

- Use flyback-protected driver stages for valves and pump relay/MOSFET paths.
- Do not drive 12V loads directly from MCU pins.
- `pressure_sensor` is optional and feature-gated in firmware.
