# Engineering Wiring Diagrams (Repo-Derived)

## Scope and source of truth

This document is generated from the board pin headers, LCD pin map, and hardware docs in this repository. No external hardware was added.

Primary sources:

- `include/pins_uno.h`
- `include/pins_nano.h`
- `include/pins_mega2560.h`
- `include/pin_map_lcd.h`
- `include/pin_policy_guards.h`
- `include/interrupt_guards.h`
- `src/arduino_adapters.cpp`
- `HARDWARE.md`

## 1) Hardware inventory

### MCU targets

- Arduino Uno (`ARDUINO_AVR_UNO`)
- Arduino Nano (`ARDUINO_AVR_NANO`)
- Arduino Mega 2560 (`ARDUINO_AVR_MEGA2560`)

### Display

- ST7920 128x64 LCD in serial 3-wire mode via U8g2 software SPI (`U8G2_ST7920_128X64_F_SW_SPI` / `_1_SW_SPI`)

### Inputs

- Run/Hold switch (active-low with pull-up)
- 3x section switches (active-low with pull-up)
- Rotary encoder:
  - Mega: CLK/DT/SW allocated
  - Nano: CLK/DT allocated, SW unassigned
  - Uno: unassigned
- Buttons (UP/DOWN/CAL/SELECT/AUTO-MANUAL):
  - Mega: allocated
  - Uno/Nano: unassigned (`0xFF` sentinel)

### Sensors

- Wheel pulse sensor (interrupt input)
- Flow pulse sensor (interrupt input)
- Pressure sensor input (optional feature-gated analog input; disabled by default)

### Outputs

- Pump PWM output
- 3x section relay outputs
- 1x section indicator LED output

### Power system

- MCU logic rail: 5V
- Pump/actuator rail: 12V (driven through external driver stage)
- Common ground required between all modules

## 2) Pin mapping tables (build-first)

Direction legend:

- `IN`: MCU input
- `OUT`: MCU output
- `ALT`: board/system reserved function (bus/UART/etc.)
- `N/C`: unassigned by firmware profile

### 2.1 Functional signal map (all defined repo signals)

| Signal | Uno | Nano | Mega2560 | Direction | Module | Notes |
| --- | --- | --- | --- | --- | --- | --- |
| Wheel sensor | D2 | D2 | D18 | IN | Pulse sensor | Interrupt-capable pin required by compile guards |
| Flow sensor | D3 | D3 | D19 | IN | Pulse sensor | Interrupt-capable pin required by compile guards |
| Pump PWM | D9 | D9 | D44 | OUT | Pump driver | 5V PWM to external 12V stage |
| Section relay 1 | D12 | D12 | D22 | OUT | Section output | Active-high output |
| Section relay 2 | D7 | D7 | D23 | OUT | Section output | Active-high output |
| Section relay 3 | A1 | A1 | D24 | OUT | Section output | Digital output on analog pin (Uno/Nano) |
| Section LED 1 | A2 | A2 | D25 | OUT | Indicator | Active-high output |
| Run/Hold switch | A3 | A3 | D26 | IN | Input switch | Active-low with INPUT_PULLUP |
| Section switch 1 | A4 | A4 | D27 | IN | Input switch | Active-low with INPUT_PULLUP |
| Section switch 2 | A5 | A5 | D28 | IN | Input switch | Active-low with INPUT_PULLUP |
| Section switch 3 | D4 | D4 | D29 | IN | Input switch | Active-low with INPUT_PULLUP |
| Encoder CLK | N/C (`0xFF`) | D5 | D30 | IN | Encoder | Nano/Mega only |
| Encoder DT | N/C (`0xFF`) | D6 | D31 | IN | Encoder | Nano/Mega only |
| Encoder SW | N/C (`0xFF`) | N/C (`0xFF`) | D32 | IN | Encoder switch | Mega only |
| Button UP | N/C (`0xFF`) | N/C (`0xFF`) | D33 | IN | Operator button | Mega only |
| Button DOWN | N/C (`0xFF`) | N/C (`0xFF`) | D34 | IN | Operator button | Mega only |
| Button CAL | N/C (`0xFF`) | N/C (`0xFF`) | D35 | IN | Operator button | Mega only |
| Button SELECT | N/C (`0xFF`) | N/C (`0xFF`) | D36 | IN | Operator button | Mega only |
| Button AUTO/MANUAL | N/C (`0xFF`) | N/C (`0xFF`) | D37 | IN | Operator button | Mega only |
| Pressure sensor (optional) | A0 | A0 | A8 | IN | Analog sensor | Feature disabled by default |
| LCD SCLK | D13 | D13 | D52 | ALT/OUT | ST7920 serial | E/clock line |
| LCD MOSI | D11 | D11 | D51 | ALT/OUT | ST7920 serial | RW/SID data line |
| LCD CS | D10 | D10 | D53 | OUT | ST7920 control | RS/chip select line |
| LCD RESET | D8 | D8 | D49 | OUT | ST7920 control | LCD reset |

### 2.2 Complete pin occupancy (Mega2560, recommended full-feature build)

| MCU Pin | Direction | Assigned Signal / Role | Notes |
| --- | --- | --- | --- |
| D0 | ALT | UART RX (reserved) | Programming/telemetry serial; do not repurpose |
| D1 | ALT | UART TX (reserved) | Programming/telemetry serial; do not repurpose |
| D2 | N/C | Unused by firmware | Interrupt-capable spare |
| D3 | N/C | Unused by firmware | Interrupt-capable spare |
| D4 | N/C | Unused by firmware | Spare digital I/O |
| D5 | N/C | Unused by firmware | PWM-capable spare |
| D6 | N/C | Unused by firmware | PWM-capable spare |
| D7 | N/C | Unused by firmware | Spare digital I/O |
| D8 | N/C | Unused by firmware | Spare digital I/O |
| D9 | N/C | Unused by firmware | PWM-capable spare |
| D10 | N/C | Unused by firmware | SPI SS spare |
| D11 | N/C | Unused by firmware | Digital spare |
| D12 | N/C | Unused by firmware | SPI MISO spare |
| D13 | N/C | Unused by firmware | LED / digital spare |
| D14 | N/C | UART3 TX spare | ALT capable |
| D15 | N/C | UART3 RX spare | ALT capable |
| D16 | N/C | UART2 TX spare | ALT capable |
| D17 | N/C | UART2 RX spare | ALT capable |
| D18 | IN | Wheel sensor pulse | External interrupt |
| D19 | IN | Flow sensor pulse | External interrupt |
| D20 | N/C | I2C SDA spare | ALT capable |
| D21 | N/C | I2C SCL spare | ALT capable |
| D22 | OUT | Section relay 1 | Digital output |
| D23 | OUT | Section relay 2 | Digital output |
| D24 | OUT | Section relay 3 | Digital output |
| D25 | OUT | Section LED 1 | Indicator output |
| D26 | IN | Run/Hold switch | INPUT_PULLUP active-low |
| D27 | IN | Section switch 1 | INPUT_PULLUP active-low |
| D28 | IN | Section switch 2 | INPUT_PULLUP active-low |
| D29 | IN | Section switch 3 | INPUT_PULLUP active-low |
| D30 | IN | Encoder CLK | INPUT_PULLUP active-low input behavior |
| D31 | IN | Encoder DT | INPUT_PULLUP active-low input behavior |
| D32 | IN | Encoder SW | INPUT_PULLUP active-low input behavior |
| D33 | IN | Button UP | INPUT_PULLUP active-low |
| D34 | IN | Button DOWN | INPUT_PULLUP active-low |
| D35 | IN | Button CAL | INPUT_PULLUP active-low |
| D36 | IN | Button SELECT | INPUT_PULLUP active-low |
| D37 | IN | Button AUTO/MANUAL | INPUT_PULLUP active-low |
| D38 | N/C | Unused by firmware | Spare digital I/O |
| D39 | N/C | Unused by firmware | Spare digital I/O |
| D40 | N/C | Unused by firmware | Spare digital I/O |
| D41 | N/C | Unused by firmware | Spare digital I/O |
| D42 | N/C | Unused by firmware | Spare digital I/O |
| D43 | N/C | Unused by firmware | Spare digital I/O |
| D44 | OUT | Pump PWM | PWM output to external driver |
| D45 | N/C | Unused by firmware | PWM-capable spare |
| D46 | N/C | Unused by firmware | PWM-capable spare |
| D47 | N/C | Unused by firmware | Spare digital I/O |
| D48 | N/C | Unused by firmware | Spare digital I/O |
| D49 | OUT | LCD RESET | Reserved for ST7920 |
| D50 | N/C | SPI MISO spare | Reserved if SPI expansion added |
| D51 | ALT/OUT | LCD MOSI | Reserved for ST7920 SID/RW |
| D52 | ALT/OUT | LCD SCLK | Reserved for ST7920 E/SCLK |
| D53 | OUT | LCD CS | Reserved for ST7920 |
| A0..A7 | N/C | Unused by firmware | Analog spares |
| A8 | IN | Pressure sensor (optional) | Analog input, feature-gated |
| A9..A15 | N/C | Unused by firmware | Analog spares |

### 2.3 Complete pin occupancy (Uno/Nano profile)

| MCU Pin | Uno | Nano | Direction | Assigned Signal / Role | Notes |
| --- | --- | --- | --- | --- | --- |
| D0 | UART RX (reserved) | UART RX (reserved) | ALT | Serial console/programming | Do not repurpose |
| D1 | UART TX (reserved) | UART TX (reserved) | ALT | Serial console/programming | Do not repurpose |
| D2 | Wheel sensor | Wheel sensor | IN | Interrupt pulse input | External interrupt |
| D3 | Flow sensor | Flow sensor | IN | Interrupt pulse input | External interrupt |
| D4 | Section switch 3 | Section switch 3 | IN | Active-low switch | INPUT_PULLUP |
| D5 | N/C | Encoder CLK | IN (Nano) | Encoder A | Uno unassigned |
| D6 | N/C | Encoder DT | IN (Nano) | Encoder B | Uno unassigned |
| D7 | Section relay 2 | Section relay 2 | OUT | Section output | Active-high |
| D8 | LCD RESET | LCD RESET | OUT | ST7920 reset | Reserved |
| D9 | Pump PWM | Pump PWM | OUT | PWM to pump driver | External driver required |
| D10 | LCD CS | LCD CS | OUT | ST7920 chip select | Reserved |
| D11 | LCD MOSI | LCD MOSI | ALT/OUT | ST7920 SID/RW data | Serial mode |
| D12 | Section relay 1 | Section relay 1 | OUT | Section output | Active-high |
| D13 | LCD SCLK | LCD SCLK | ALT/OUT | ST7920 E/SCLK clock | Serial mode |
| A0 | Pressure sensor (optional) | Pressure sensor (optional) | IN | Analog pressure | Feature-gated |
| A1 | Section relay 3 | Section relay 3 | OUT | Section output | Analog pin used as digital out |
| A2 | Section LED 1 | Section LED 1 | OUT | Indicator output | Analog pin used as digital out |
| A3 | Run/Hold switch | Run/Hold switch | IN | Active-low switch | INPUT_PULLUP |
| A4 | Section switch 1 | Section switch 1 | IN | Active-low switch | INPUT_PULLUP |
| A5 | Section switch 2 | Section switch 2 | IN | Active-low switch | INPUT_PULLUP |
| A6 | N/C | N/C (analog-only) | N/C | Unused | Nano A6/A7 avoided for digital |
| A7 | N/C | N/C (analog-only) | N/C | Unused | Nano A6/A7 avoided for digital |

Unassigned logical roles on Uno/Nano profile:

- Uno: Encoder CLK/DT/SW and all five UI buttons are intentionally unassigned (`0xFF`).
- Nano: Encoder SW and all five UI buttons are intentionally unassigned (`0xFF`).

## 3) System wiring diagram (high-level)

```text
+-------------------- POWER --------------------+
| 12V rail -> Pump driver + section actuators   |
|  5V rail -> MCU + LCD + sensors/switches      |
| GND common -> ALL modules                     |
+----------------------|------------------------+
                       v
+--------------------------------------------------------------+
|                         MCU (Uno/Nano/Mega)                  |
|                                                              |
| Inputs (left):                    Outputs (right):           |
| - Wheel pulse sensor              - Pump PWM -> driver       |
| - Flow pulse sensor               - Section relay 1..3       |
| - Run/Hold + section switches     - Section indicator LED    |
| - Encoder/buttons (board profile)                            |
| - Optional pressure analog input                              |
|                                                              |
| Top buses: LCD serial (E/SCLK, SID/RW, CS/RS, RST)          |
| Bottom: common GND                                            |
+--------------------------------------------------------------+
```

## 4) Pin-level wiring diagram (critical)

### 4.1 Mega2560 full-feature profile

```text
                      +5V rail
                        |
       +----------------+----------------+
       |                                 |
[Inputs, left]                       [Outputs, right]
Wheel sensor OUT -----> D18 (IN/INT)      D44 (PWM OUT) -----> Pump driver PWM IN
Flow sensor OUT  -----> D19 (IN/INT)      D22 (OUT) ---------> Section relay 1 IN
Run/Hold SW -----+----> D26 (IN_PU)       D23 (OUT) ---------> Section relay 2 IN
Section SW1 -----+----> D27 (IN_PU)       D24 (OUT) ---------> Section relay 3 IN
Section SW2 -----+----> D28 (IN_PU)       D25 (OUT) ---------> Section LED+
Section SW3 -----+----> D29 (IN_PU)
Encoder CLK -----+----> D30 (IN_PU)
Encoder DT ------+----> D31 (IN_PU)
Encoder SW ------+----> D32 (IN_PU)
BTN_UP ----------+----> D33 (IN_PU)
BTN_DOWN --------+----> D34 (IN_PU)
BTN_CAL ---------+----> D35 (IN_PU)
BTN_SELECT ------+----> D36 (IN_PU)
BTN_A/M ---------+----> D37 (IN_PU)
Pressure OUT ----------> A8 (ANALOG IN, optional)

[Top bus: LCD serial]
D52 (SCLK) -----------> ST7920 SCLK/E
D51 (MOSI) -----------> ST7920 SID/RW
D53 (CS) -------------> ST7920 CS/RS
D49 (RST) ------------> ST7920 RST
ST7920 PSB -----------> GND (force serial mode)
ST7920 DB0..DB7 ------> N/C (unused in serial mode)

[Bottom bus: GND]
MCU GND --------------> LCD GND, sensor GND, switch returns, driver grounds
```

### 4.2 Uno/Nano profile

```text
Inputs left:                           Outputs right:
D2  <- Wheel pulse (INT)               D9  -> Pump PWM -> external driver
D3  <- Flow pulse  (INT)               D12 -> Section relay 1
D4  <- Section SW3 (IN_PU)             D7  -> Section relay 2
A3  <- Run/Hold (IN_PU)                A1  -> Section relay 3
A4  <- Section SW1 (IN_PU)             A2  -> Section LED
A5  <- Section SW2 (IN_PU)
A0  <- Pressure analog (optional)
Nano-only: D5 <- Enc CLK, D6 <- Enc DT

LCD serial bus:
D13 -> ST7920 SCLK/E
D11 -> ST7920 SID/RW
D10 -> ST7920 CS/RS
D8  -> ST7920 RST
PSB -> GND

Common GND across MCU/LCD/sensors/actuator drivers.
```

## 5) Harness / connector diagram

Assigned connector IDs and wire colors (color set standardized because repo does not define colors):

| Connector | Pin | Wire Color | Function | Endpoint |
| --- | --- | --- | --- | --- |
| J1 POWER | 1 | Red | +12V input | External power source |
| J1 POWER | 2 | Black | GND | External power source |
| J2 MCU_5V | 1 | Red/White | +5V logic rail | MCU/LCD/sensors |
| J2 MCU_5V | 2 | Black | GND | MCU/LCD/sensors |
| J3 FLOW | 1 | Brown | +5V sensor supply | Flow sensor |
| J3 FLOW | 2 | Blue | GND | Flow sensor |
| J3 FLOW | 3 | White | Flow pulse signal | MCU flow pin |
| J4 WHEEL | 1 | Brown | +5V sensor supply | Wheel sensor |
| J4 WHEEL | 2 | Blue | GND | Wheel sensor |
| J4 WHEEL | 3 | Green | Wheel pulse signal | MCU wheel pin |
| J5 RUN_HOLD | 1 | Gray | Switch signal | MCU Run/Hold pin |
| J5 RUN_HOLD | 2 | Black | GND return | Chassis/MCU GND |
| J6 SECTION_SW | 1 | Gray/White | SW1 signal | MCU SW1 pin |
| J6 SECTION_SW | 2 | Gray/Blue | SW2 signal | MCU SW2 pin |
| J6 SECTION_SW | 3 | Gray/Green | SW3 signal | MCU SW3 pin |
| J6 SECTION_SW | 4 | Black | GND common | Chassis/MCU GND |
| J7 LCD | 1 | Red/White | +5V | ST7920 VCC |
| J7 LCD | 2 | Black | GND | ST7920 GND |
| J7 LCD | 3 | Yellow | SCLK | ST7920 SCLK/E |
| J7 LCD | 4 | Orange | MOSI | ST7920 SID/RW |
| J7 LCD | 5 | Purple | CS | ST7920 CS/RS |
| J7 LCD | 6 | White/Black | RST | ST7920 RST |
| J7 LCD | 7 | Black/White | PSB strap to GND | ST7920 PSB |
| J8 PUMP_DRV | 1 | Violet | PWM command | Driver control input |
| J8 PUMP_DRV | 2 | Black | GND reference | Driver ground |
| J9 SECTION_OUT | 1 | Pink | Section 1 control | Solenoid/relay 1 driver |
| J9 SECTION_OUT | 2 | Pink/Blue | Section 2 control | Solenoid/relay 2 driver |
| J9 SECTION_OUT | 3 | Pink/Green | Section 3 control | Solenoid/relay 3 driver |
| J9 SECTION_OUT | 4 | Black | GND reference | Driver ground |

Harness construction notes:

- Group J3/J4/J5/J6 low-current input wiring separately from J8/J9 actuator command harness.
- Use twisted pair for each pulse sensor signal+ground pair (J3-3 with J3-2, J4-3 with J4-2).
- If operating in noisy environments near pump motors, use shielded cable for J3/J4 and terminate shield to chassis ground at one end only.

## 6) Electrical assumptions (explicit)

- Pull-ups: firmware configures switch and interrupt inputs using `INPUT_PULLUP`; expected field switches short to GND when active.
- Debounce: implemented in software with `INPUT_DEBOUNCE_MS = 30 ms`; no hardware debounce required by firmware contract.
- Voltage compatibility: all MCU-side I/O is 5V logic (Uno/Nano/Mega AVR).
- Current driving: GPIO lines are logic-only control lines; pump and section actuators must be
  driven through external transistor/MOSFET/relay stages (not directly from MCU pin).
- Pressure sensor: analog telemetry path exists but is disabled by default (`ENABLE_PRESSURE_SENSOR = false`).

## 7) Validation checks

### 7.1 Pin conflicts

- Compile-time guard enforces uniqueness of assigned roles and uniqueness of UI pins when allocated.
- Compile-time guard reserves LCD/SPI pins from UI pin overlap.
- Uno/Nano repeated `0xFF` entries are intentionally unassigned sentinels, not physical conflicts.

### 7.2 Electrical risks

- Risk: direct-driving inductive loads from GPIO would overcurrent pins. Mitigation: mandatory external drivers + flyback handling.
- Risk: floating inputs if switches not wired to GND return. Mitigation: use active-low wiring with common ground.
- Risk: sensor line noise causing false pulses. Mitigation: shield/twist pulse lines, keep grounds low-impedance.

### 7.3 Interface correctness

- ST7920 LCD: repo maps serial clock/data and dedicated CS/RESET per board profile; PSB must be tied LOW.
- Interrupt pins: flow and wheel pins are compile-time constrained to external interrupt-capable pins for each board family.

## 8) Wiring table (mandatory)

| From | To | Signal | Notes |
| --- | --- | --- | --- |
| Wheel sensor signal | MCU wheel pin (D2 Uno/Nano, D18 Mega) | WHEEL_PULSE | Interrupt input |
| Flow sensor signal | MCU flow pin (D3 Uno/Nano, D19 Mega) | FLOW_PULSE | Interrupt input |
| Pump driver PWM input | MCU pump pin (D9 Uno/Nano, D44 Mega) | PUMP_PWM | 5V PWM logic |
| Section driver CH1 input | Section relay pin 1 | SECTION1_CMD | Digital output |
| Section driver CH2 input | Section relay pin 2 | SECTION2_CMD | Digital output |
| Section driver CH3 input | Section relay pin 3 | SECTION3_CMD | Digital output |
| Run/Hold switch terminal | MCU Run/Hold pin | RUN_HOLD_SW | Active-low, INPUT_PULLUP |
| Section switch 1 terminal | MCU SW1 pin | SECTION_SW1 | Active-low, INPUT_PULLUP |
| Section switch 2 terminal | MCU SW2 pin | SECTION_SW2 | Active-low, INPUT_PULLUP |
| Section switch 3 terminal | MCU SW3 pin | SECTION_SW3 | Active-low, INPUT_PULLUP |
| Encoder CLK (if allocated) | MCU encoder CLK pin | ENC_A | Nano/Mega profile |
| Encoder DT (if allocated) | MCU encoder DT pin | ENC_B | Nano/Mega profile |
| Encoder SW (if allocated) | MCU encoder SW pin | ENC_SW | Mega profile |
| MCU LCD SCLK pin | ST7920 SCLK/E | LCD_SCLK | Serial clock |
| MCU LCD MOSI pin | ST7920 SID/RW | LCD_MOSI | Serial data |
| MCU LCD CS pin | ST7920 CS/RS | LCD_CS | LCD chip select |
| MCU LCD RESET pin | ST7920 RST | LCD_RST | LCD reset |
| ST7920 PSB | GND | LCD_PSB_MODE | Must be LOW for serial mode |
| ST7920 DB0..DB7 | N/C | LCD_PARALLEL_BUS | Must be unused in serial mode |
| +5V rail | MCU + LCD + sensors | +5V | Logic supply |
| +12V rail | Pump and valve power stage | +12V_ACT | High-power rail |
| System grounds | All module grounds | GND_COMMON | Mandatory common reference |

## 9) Issues table

| Severity | Issue | Location | Fix |
| --- | --- | --- | --- |
| High | Potential GPIO damage if pump/valve loads are wired directly to MCU pins | Pump/section outputs | Use external MOSFET/relay drivers and flyback diodes |
| Medium | False pulse counts in high EMI routing | Flow/wheel harness | Use twisted/shielded pairs and clean grounding |
| Medium | Inputs read incorrect state if ground return omitted | All active-low switches | Ensure each switch has solid GND return |
| Low | Uno has no allocated encoder/buttons in firmware profile | Uno UI inputs | Use Mega profile for full UI or remap pins with firmware changes |
| Low | Pressure channel is wired but feature-disabled by default | A0/A8 pressure input | Enable `ENABLE_PRESSURE_SENSOR` only after calibration/validation |
