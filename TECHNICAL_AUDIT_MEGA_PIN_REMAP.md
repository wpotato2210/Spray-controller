# Arduino Mega (ATmega2560) Pin Remap Technical Audit

Date: 2026-03-24 (UTC)

## 1) Summary

**Can non-LCD pins be safely remapped for Arduino Mega?**

### Answer

Conditional No (under strict D22–D53 + A8–A15-only policy).

- Most GPIO functions (boom outputs, switches, LED, optional pressure analog input) can be remapped safely into `D22–D53` and `A8–A15`.
- **Wheel and flow pulse inputs cannot** be remapped into that range **without firmware redesign**, because the current firmware uses
  `attachInterrupt(digitalPinToInterrupt(pin))`, and Mega external interrupts
  are only on `D2, D3, D18, D19, D20, D21`.
- Therefore a safe migration is possible only if you allow two exceptions
  (wheel/flow on interrupt-capable pins outside D22–D53/A8–A15), or if you
  refactor to pin-change interrupts / polling hardware.

## 2) Current pin usage

### Source-of-truth definitions (declared)

Current profile definitions in `pins.yaml`:

- `wheel_sensor`, `flow_sensor` (pulse inputs)
- `pump_pwm` (PWM output)
- `boom_sections.section_[1..3]` (digital outputs)
- `leds.section_1` (digital output)
- `switches.run_hold`, `switches.section_switches.section_[1..3]` (digital inputs with pull-up)

### Firmware-compiled mapping (actual build path)

The compiled firmware currently reads board-specific constants from
`include/pins_uno.h` and `include/pins_nano.h`; `pins.yaml` is a
documentation/config artifact, not a compile-time source.

| Function | UNO/Nano Pin | Usage class | Pin-type requirement |
| --- | ---: | --- | --- |
| Wheel sensor | D2 | Interrupt pulse input | External interrupt-capable pin |
| Flow sensor | D3 | Interrupt pulse input | External interrupt-capable pin |
| Pump control | D11 | PWM output | PWM-capable pin |
| Boom section 1 | D12 | Digital output | Any digital output |
| Boom section 2 | D13 | Digital output | Any digital output |
| Boom section 3 | A1 | Digital output | Any digital output |
| Section 1 LED | A2 | Digital output | Any digital output |
| Run/Hold switch | A3 | Digital input pull-up | Any digital input |
| Section switch 1 | A4 | Digital input pull-up | Any digital input |
| Section switch 2 | A5 | Digital input pull-up | Any digital input |
| Section switch 3 | D4 | Digital input pull-up | Any digital input |
| Pressure sensor (optional) | A0 | Analog input | ADC-capable pin |

## 3) Code dependency tracing (config-driven design audit)

### Findings

1. **Hardcoded compile-time pin mappings exist in C++ headers**
   (`include/pins_uno.h`, `include/pins_nano.h`) and are used by runtime
   objects in `Spray-controller.ino` / adapters.
2. Firmware does **not parse/use `pins.yaml` at runtime or build-generation stage**.
3. Interrupt guards are explicitly UNO/Nano-centric for D2/D3 in `include/interrupt_guards.h`.

### Direct pin I/O and interrupt calls

- `pinMode`, `digitalRead`, `digitalWrite`, `analogRead`, `analogWrite`, `attachInterrupt`, `digitalPinToInterrupt` are all centralized in `src/arduino_adapters.cpp`.
- Application logic uses named constants (`PIN_*`) rather than scattered literals, which is good.
- **Violation vs requested config-driven model**: pin definitions are split
  between `pins.yaml` and `include/pins_*.h`, with the latter actually
  controlling firmware behavior.

## 4) Hardware constraints (from HARDWARE.md / BOM.md)

### Explicit constraints in repo

- Flow + wheel sensors are pulse-output devices.
- Pump is PWM-compatible and expects PWM drive.
- Optional pressure sensor is analog telemetry input.
- Build intentionally rejects non-UNO/Nano boards today (`#error` in `include/pins.h`).

### Bus and board reservations

- Current firmware does not use `Wire`/`SPI` libraries for control I/O.
- Serial (`Serial`) is used for telemetry/commands.
- If an LCD shield is present, interface type is **not documented** in focus
  files; assume uncertainty and avoid claiming free ownership of typical shield
  pins without board-level verification.

## 5) Mega compatibility analysis per function

| Function | Can move to D22–D53/A8–A15 only? | Status | Reason |
| --- | --- | --- | --- |
| Wheel sensor pulse input | No | **BLOCKED** | Current firmware requires external interrupt pin via `attachInterrupt`; Mega ext-interrupt pins are D2/3/18/19/20/21 only. |
| Flow sensor pulse input | No | **BLOCKED** | Same as wheel sensor. |
| Pump PWM | Yes (e.g., D44/45/46) | **CONDITIONAL** | Must stay on PWM-capable Mega pin. |
| Boom section outputs | Yes | **SAFE** | Plain digital outputs. |
| LED output | Yes | **SAFE** | Plain digital output. |
| Run/Hold switch | Yes | **SAFE** | Digital input pull-up. |
| Section switches | Yes | **SAFE** | Digital input pull-up. |
| Optional pressure sensor | Yes (A8–A15) | **SAFE** | ADC-capable pin required. |

### Timer/library hidden risk check

- No explicit timer register coupling found for pump PWM; code uses `analogWrite` abstraction.
- PWM frequency/timer identity can still vary by pin/timer on Mega; validate pump driver behavior electrically.
- Interrupt logic uses `digitalPinToInterrupt` (portable), but static asserts
  are UNO/Nano-specific and must be updated for Mega support.

## 6) Proposed Mega pin mapping (`pins.yaml` rewrite)

The following profile was added as proposal:

```yaml
  arduino_avr_mega2560:
    # BLOCKED from D22-D53/A8-A15-only policy under current firmware:
    # flow/wheel rely on attachInterrupt(digitalPinToInterrupt(pin)).
    # On Mega2560 external interrupts are fixed to D2, D3, D18, D19, D20, D21.
    wheel_sensor: 18
    flow_sensor: 19
    # PWM required for pump output (Mega PWM-capable in requested range: D44-D46).
    pump_pwm: 44

    boom_sections:
      section_1: 22
      section_2: 23
      section_3: 24

    leds:
      section_1: 25

    switches:
      run_hold: 26
      section_switches:
        section_1: 27
        section_2: 28
        section_3: 29

    pressure_sensor: A8
```

## 7) Required code changes for true Mega migration

1. Add `include/pins_mega2560.h` and extend `include/pins.h` target selection for `ARDUINO_AVR_MEGA2560`.
2. Update `include/interrupt_guards.h`:
   - Replace UNO/Nano D2/D3 static asserts with a Mega-compatible rule, or a
     generic `digitalPinToInterrupt(...) != NOT_AN_INTERRUPT`
     compile/run-time validation strategy.
3. Decide and document single source of truth:
   - either generate `include/pins_<board>.h` from `pins.yaml` in CI,
   - or deprecate `pins.yaml` as non-authoritative documentation to avoid drift.
4. If strict policy requires **all** non-LCD pins inside D22–D53/A8–A15,
   refactor pulse counting away from external interrupts (e.g., pin-change
   interrupts with Mega-specific implementation and validation).
5. Keep bench app transition semantics as-is: runtime state commit should occur
   only in `_on_controller_state_entered`, preserving illegal transition
   consistency.

## 8) Risk level

### Medium

- Medium rather than low because migration requires board-target support changes and interrupt-constraint handling.
- Not high because most I/O is straightforward digital/analog remap once interrupt pins are handled explicitly.
