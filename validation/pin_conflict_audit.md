# Arduino Pin Conflict Audit — Spraymate Clone Firmware

Date: 2026-04-07 (UTC)

## Scope and method

Files parsed:

- All firmware source files: `Spray-controller.ino`, `config.h`, all `include/*.h`, and all `src/*.cpp`.
- Pin declarations were extracted from board pin maps
  (`include/pins_uno.h`, `include/pins_nano.h`) and traced
  through constructors, adapter initialization, and `pinMode`
  / interrupt usage.
- Display, encoder, and physical menu-button assignments were
  explicitly searched for (`U8g2`, `U8glib`, `ST7920`, `encoder`,
  `rotary`, `CLK`, `DT`, `SW`, `LCD`, `SPI`) with no direct
  hardware pin mappings found.

## Board target reality check

The firmware currently supports only UNO/NANO board targets:

- `include/pins.h` errors out for non-UNO/NANO builds.
- There is no Mega 2560 pin map header in this repo.

Therefore, Mega 2560 validation in this audit is compatibility analysis of current mappings (not a native Mega pin profile).

## 1) Unified Pin Table (deterministic extraction)

| Pin | Module | Signal | Direction | Notes |
|---|---|---|---|---|
| 2 | Sensor | Wheel sensor pulse input | IN (INPUT_PULLUP + interrupt) | `PIN_WHEEL_SENSOR`; interrupt attached on `RISING` |
| 3 | Sensor | Flow sensor pulse input | IN (INPUT_PULLUP + interrupt) | `PIN_FLOW_SENSOR`; interrupt attached on `RISING` |
| 11 | Actuation | Pump PWM | OUT (PWM) | `PIN_PUMP_PWM`; written via `analogWrite` |
| 12 | Section control | Boom section 1 output | OUT | `PIN_SECTION_RELAY_1`; active-high output |
| 13 | Section control | Boom section 2 output | OUT | `PIN_SECTION_RELAY_2`; also board LED/SCK on UNO/NANO hardware |
| A1 | Section control | Boom section 3 output | OUT | `PIN_SECTION_RELAY_3` |
| A2 | Indicator | Section 1 indicator output | OUT | `PIN_LED_SECTION_1`; mirrors section 0 state |
| A3 | User input | RUN/HOLD switch | IN (INPUT_PULLUP) | `PIN_RUN_HOLD`; active-low logic |
| A4 | User input | Section switch 1 input | IN (INPUT_PULLUP) | `PIN_SECTION_SW_1`; active-low logic |
| A5 | User input | Section switch 2 input | IN (INPUT_PULLUP) | `PIN_SECTION_SW_2`; active-low logic |
| 4 | User input | Section switch 3 input | IN (INPUT_PULLUP) | `PIN_SECTION_SW_3`; active-low logic |
| A0 | Sensor (optional) | Pressure analog input | IN (INPUT + `analogRead`) | `PIN_PRESSURE_SENSOR`; feature currently disabled by config |

### Not present in codebase (explicitly unresolved/ambiguous)

- Hall effect sensors beyond wheel/flow pulse counters: **not declared**.
- Rotary encoder pins (CLK/DT/SW): **not declared**.
- Discrete menu button pins (UP/DOWN/SELECT/CAL/AUX): **not declared**.
- Dedicated boom switch bank beyond section switch inputs above: no additional assignments.
- 12864 LCD/ST7920 pins: **no display library, no LCD constructor, no interface pins defined**.

## 2) Conflict Report

## CRITICAL

1. **Board profile mismatch vs requested Mega 2560 assumption**
   - **Pin(s):** entire mapping set
   - **Conflicting modules:** N/A (build-target conflict)
   - **Root cause:** `include/pins.h` rejects non-UNO/NANO targets; there is no Mega pin map.
   - **Impact:** Cannot claim hardware-valid Mega deployment from current source as-is.

## WARNING

1. **Potential SPI contention on pin 13 if SPI LCD/device is added later**
   - **Pin:** 13
   - **Conflicting modules:** Boom section output vs hardware SCK semantics on UNO/NANO and ISP programming path.
   - **Root cause:** `PIN_SECTION_RELAY_2` mapped to D13.
   - **Impact:** Any later SPI peripheral wiring (including potential ST7920 SPI mode on UNO/NANO-style SPI pins) could conflict electrically/functionally.

2. **Interrupt policy guard is UNO/NANO-specific, not Mega-specific**
   - **Pin(s):** 2, 3
   - **Conflicting modules:** Flow/Wheel pulse inputs and board portability requirements.
   - **Root cause:** Compile-time assertions only enforce UNO/NANO interrupt-capable pins (D2/D3).
   - **Impact:** No explicit policy for Mega interrupt-pin set if Mega support is introduced.

## PASS (no direct conflicts found in current active map)

- No duplicate pin assignment across active modules.
- No pin used simultaneously as input and output.
- No use of Serial pins D0/D1 in pin map.
- No use of Mega hardware SPI pins 50/51/52/53.
- No use of Mega I2C pins 20/21.

## 3) LCD-specific Check

- **Library detection:** No `U8g2`, `U8glib`, `LiquidCrystal`, `ST7920`, or other LCD driver initialization found.
- **Interface mode inference:** Not inferable (no constructors/macros for SPI or parallel LCD).
- **Illegal SPI/parallel mixing:** Not applicable; LCD subsystem absent in analyzed firmware.
- **Result:** LCD pin audit is **blocked by missing implementation** (explicit ambiguity, not assumed).

## 4) Recommendations (remediation plan)

1. Add explicit Mega profile before deploying on Mega:
   - Create `include/pins_mega.h` and route via `include/pins.h`.
   - Keep pulse inputs on Mega interrupt-capable pins (2, 3, 18, 19, 20, 21).

2. Reserve comms buses intentionally:
   - Avoid D13 for non-SPI-critical outputs if SPI devices/LCD may be added.
   - Pre-reserve Mega SPI (50–53) and I2C (20/21) in documentation.

3. Separate UI input namespace from section switches:
   - Define explicit constants for encoder/menu controls (`PIN_ENC_CLK`, `PIN_ENC_DT`, `PIN_ENC_SW`, etc.).
   - For Mega, prefer interrupt-capable pins for encoder CLK/DT.

4. Add compile-time pin validation:
   - Static checks for uniqueness of all assigned pins.
   - Static checks that pulse/encoder channels map to interrupt-capable pins for active board target.

5. LCD integration hygiene (when added):
   - Use one mode only (ST7920 SPI *or* parallel).
   - Validate constructor pin order against chosen library documentation.
   - Add compile-time guard to prevent mixed-mode pin definitions.

## 5) Final Verdict

# **FAIL**

Conflicts within the currently active UNO/NANO map are clean,
but the overall requested audit target (Mega 2560 hardware-valid
mapping including LCD/encoder/menu input subsystems) is incomplete
due to missing board profile and missing UI/display pin
implementations.
