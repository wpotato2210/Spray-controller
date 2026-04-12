# Full Engineering Audit + Refactor Recommendations (2026-04-12)

## A) Executive Summary

### Top 5 systemic risks

1. **Mega pin-map conflict is present in source and docs**: `PIN_BTN_SELECT`
   and `LCD_RESET` both use D49 on Mega, creating a hard contention if UI
   input is enabled.  
2. **Input integrity is weak**: run/hold and section switches are sampled raw with
   no debounce/filtering, so contact bounce can produce actuator chatter and
   unstable section masks.  
3. **Telemetry emission is not transaction-safe**: capacity is checked once before
   a multi-frame burst; serial buffer can still saturate mid-batch and skew frame
   cadence/order under load.  
4. **Architecture/runtime mismatch**: architecture claims strict control-core
   purity, but several control classes include Arduino headers and call `millis()`
   directly, reducing portability/testability.  
5. **Documentation drift around board support and dependencies**: README states
   only UNO/NANO support and no external libraries, while code supports Mega and
   uses U8g2 for LCD.

### Top 5 engineering weaknesses

1. **No explicit event queue/backpressure model** for menu/command handling;
   serial parser and control loop share timing budget opportunistically.  
2. **Repeated linear section lookup patterns** (`readSwitch`, `writeSection`,
   bitmask building) increase coupling and maintenance surface.  
3. **ISR timestamp strategy uses `millis()` in ISR path**, which is
   low-resolution and not ideal for high pulse rates or jitter-sensitive
   diagnostics.  
4. **Protocol constants include unused prefixes** (`FS:`, `P:`, `SW:`) without producer usage, increasing API ambiguity.  
5. **Board-specific pin policy is split between code and markdown** without
   single-source validation for all peripherals (especially UI inputs and LCD).

---

## B) Issue Table (Mandatory)

| Severity | Area | Issue | Evidence | Fix Recommendation |
| --- | --- | --- | --- | --- |
| CRITICAL | Hardware Integration | Mega pin conflict: LCD reset and button select both on D49 | `LCD_RESET=49` (Mega) and `PIN_BTN_SELECT=49` (Mega). | Remap one net immediately (preferred: move `PIN_BTN_SELECT` off SPI/LCD domain) and add compile-time uniqueness checks for all mapped pins. |
| HIGH | Hardware Integration | HARDWARE.md and pin headers disagree for Mega button pins | Docs list buttons on `33-37`; code maps `46-50`. | Regenerate hardware table from headers or enforce CI check that docs pin table matches `pins_*.h`. |
| HIGH | Firmware Quality | No debounce for run/hold and section switches | Switches use `INPUT_PULLUP` + direct `digitalRead` every loop, then directly drive section outputs. | Add per-input debounce state (e.g., 20–40 ms) in adapter or input service; only publish stable states. |
| HIGH | Firmware Quality | Serial TX capacity check only pre-batch, not per-frame | `hasTelemetryTxCapacity()` called once before publishing multiple frames in status/section/sensor burst. | Gate each frame write by available space or implement non-blocking telemetry queue with frame budgeting per loop. |
| HIGH | Architecture | Control-core portability leak (`millis()` in sensor/domain classes) | `FlowSensor`, `WheelSensor` include Arduino and use `millis()` directly. | Inject time source via interface (`TimeSourceAdapter`) to make control core deterministic/testable and bridge-friendly. |
| MEDIUM | Build/Config | README support matrix contradicts compile-time board support | README says UNO/NANO only; `pins.h` includes UNO/NANO/MEGA. | Align README with `pins.h`; add CI compile matrix for all declared targets. |
| MEDIUM | Build/Config | Dependency declaration stale: README says no external libs | `display.cpp` depends on `U8g2lib.h`. | Document U8g2 dependency and tested version range; fail build early with clear message if missing. |
| MEDIUM | State/Logic | Menu update path runs every loop with `kNone`, but no explicit queue or rate control for serial command bursts | Serial parser can emit events asynchronously; state machine update is cadence-coupled to loop timing. | Introduce bounded event queue (`N` events) with overflow telemetry + deterministic dequeue budget each cycle. |
| MEDIUM | Hardware Integration | UNO/NANO LCD + GPIO layout is tight; no guard against future SPI peripheral additions | UNO/NANO use SPI pins for LCD and also general GPIO nearby (e.g., D12 relay). | Reserve SPI domain in pin policy and add static asserts preventing non-SPI-safe assignments when LCD enabled. |
| LOW | Code Quality | Unused protocol prefixes and latent pin constants | `MSG_FLOW_PREFIX`, `MSG_PUMP_PREFIX`, `MSG_SWITCH_PREFIX` defined but not emitted; several UI pins are placeholders. | Remove/guard unused protocol tokens; explicitly mark placeholder pins in a dedicated optional-feature block. |
| LOW | Code Quality | Duplicate sanitize helper logic across modules | Similar `sanitizeNonNegativeFinite` appears in multiple translation units. | Consolidate in shared utility header for consistency and single-point behavior changes. |

---

## C) Architecture Diagram (Text Only)

```text
[GPIO/ISR Layer]
  ArduinoInterruptPulseCounter (flow, wheel ISRs)
  ArduinoActiveLowInput (switches)
  ArduinoActiveHighOutput / ArduinoPwmOutput (actuators)
  ArduinoAnalogInput (pressure)
            |
            v
[Adapter Boundary]
  DigitalInputAdapter / DigitalOutputAdapter / PulseCounterAdapter /
  AnalogInputAdapter / PwmOutputAdapter / SectionHardwareAdapter
            |
            v
[Domain Processing]
  FlowSensor + WheelSensor + PressureSensor
  RunHoldSwitch + SectionManager
  FlowController + CoverageAccumulator
  OperatorMenuStateMachine
            |
            v
[Application Orchestration]
  loop(): read -> compute -> actuate -> render -> telemetry
            |
            +--> Display (U8g2 ST7920)
            +--> Serial protocol publisher/parser
```

### Bottlenecks / unsafe coupling

- `loop()` is a monolithic scheduler; serial parsing, control, display rendering, and telemetry share one timing path.
- Command ingestion and telemetry output are both on `Serial`; no explicit arbitration/backpressure queue.
- Timing source (`millis`) is embedded inside sensor domain classes, coupling portability to Arduino core.

---

## D) Pin & Hardware Conflict Audit (Critical)

## Full pin usage table

### Arduino UNO

| Pin | Module | Function | Board dependency / notes |
| --- | --- | --- | --- |
| D2 | wheel sensor ISR | `PIN_WHEEL_SENSOR` | External interrupt-capable (valid on UNO). |
| D3 | flow sensor ISR | `PIN_FLOW_SENSOR` | External interrupt-capable (valid on UNO). |
| D4 | section switch 3 | `PIN_SECTION_SW_3` | Input pull-up. |
| D7 | section relay 2 | `PIN_SECTION_RELAY_2` | Digital output. |
| D8 | LCD | `LCD_RESET` | ST7920 reset. |
| D9 | pump control | `PIN_PUMP_PWM` | PWM output. |
| D10 | LCD | `LCD_CS` | SPI chip-select domain. |
| D11 | LCD | `LCD_MOSI` | SPI MOSI. |
| D12 | section relay 1 | `PIN_SECTION_RELAY_1` | Shared with SPI MISO pin number domain on AVR. |
| D13 | LCD | `LCD_CLK` | SPI clock. |
| A0 | pressure sensor | `PIN_PRESSURE_SENSOR` | Feature-gated, disabled by default. |
| A1 | section relay 3 | `PIN_SECTION_RELAY_3` | Digital-capable analog pin used as output. |
| A2 | indicator LED | `PIN_LED_SECTION_1` | Digital output. |
| A3 | run/hold | `PIN_RUN_HOLD` | Input pull-up. |
| A4 | section switch 1 | `PIN_SECTION_SW_1` | Input pull-up. |
| A5 | section switch 2 | `PIN_SECTION_SW_2` | Input pull-up. |
| 0xFF | UI placeholders | Encoder/buttons | Not assigned on UNO. |

### Arduino Nano

| Pin | Module | Function | Board dependency / notes |
| --- | --- | --- | --- |
| D2 | wheel sensor ISR | `PIN_WHEEL_SENSOR` | External interrupt-capable (valid on Nano). |
| D3 | flow sensor ISR | `PIN_FLOW_SENSOR` | External interrupt-capable (valid on Nano). |
| D4 | section switch 3 | `PIN_SECTION_SW_3` | Input pull-up. |
| D5 | UI placeholder | `PIN_ENCODER_CLK` | Assigned in header but not consumed by firmware logic. |
| D6 | UI placeholder | `PIN_ENCODER_DT` | Assigned in header but not consumed by firmware logic. |
| D7 | section relay 2 | `PIN_SECTION_RELAY_2` | Digital output. |
| D8 | LCD | `LCD_RESET` | ST7920 reset. |
| D9 | pump control | `PIN_PUMP_PWM` | PWM output. |
| D10 | LCD | `LCD_CS` | SPI chip-select domain. |
| D11 | LCD | `LCD_MOSI` | SPI MOSI. |
| D12 | section relay 1 | `PIN_SECTION_RELAY_1` | Shared with SPI MISO pin number domain on AVR. |
| D13 | LCD | `LCD_CLK` | SPI clock. |
| A0 | pressure sensor | `PIN_PRESSURE_SENSOR` | Feature-gated, disabled by default. |
| A1 | section relay 3 | `PIN_SECTION_RELAY_3` | Digital-capable analog pin used as output. |
| A2 | indicator LED | `PIN_LED_SECTION_1` | Digital output. |
| A3 | run/hold | `PIN_RUN_HOLD` | Input pull-up. |
| A4 | section switch 1 | `PIN_SECTION_SW_1` | Input pull-up. |
| A5 | section switch 2 | `PIN_SECTION_SW_2` | Input pull-up. |
| 0xFF | UI placeholders | encoder sw/buttons | Not assigned on Nano. |

### Arduino Mega 2560

| Pin | Module | Function | Board dependency / notes |
| --- | --- | --- | --- |
| D18 | wheel sensor ISR | `PIN_WHEEL_SENSOR` | External interrupt-capable (valid on Mega). |
| D19 | flow sensor ISR | `PIN_FLOW_SENSOR` | External interrupt-capable (valid on Mega). |
| D22 | section relay 1 | `PIN_SECTION_RELAY_1` | Digital output. |
| D23 | section relay 2 | `PIN_SECTION_RELAY_2` | Digital output. |
| D24 | section relay 3 | `PIN_SECTION_RELAY_3` | Digital output. |
| D25 | indicator LED | `PIN_LED_SECTION_1` | Digital output. |
| D26 | run/hold | `PIN_RUN_HOLD` | Input pull-up. |
| D27 | section switch 1 | `PIN_SECTION_SW_1` | Input pull-up. |
| D28 | section switch 2 | `PIN_SECTION_SW_2` | Input pull-up. |
| D29 | section switch 3 | `PIN_SECTION_SW_3` | Input pull-up. |
| D30 | UI placeholder | `PIN_ENCODER_CLK` | Assigned, not consumed by current logic. |
| D31 | UI placeholder | `PIN_ENCODER_DT` | Assigned, not consumed by current logic. |
| D32 | UI placeholder | `PIN_ENCODER_SW` | Assigned, not consumed by current logic. |
| D44 | pump control | `PIN_PUMP_PWM` | PWM output. |
| D46 | UI placeholder | `PIN_BTN_UP` | Assigned, not consumed by current logic. |
| D47 | UI placeholder | `PIN_BTN_DOWN` | Assigned, not consumed by current logic. |
| D48 | UI placeholder | `PIN_BTN_CAL` | Assigned, not consumed by current logic. |
| D49 | **CONFLICT** | `PIN_BTN_SELECT` + `LCD_RESET` | **Unsafe shared usage; critical conflict.** |
| D50 | UI placeholder | `PIN_BTN_AUTO_MANUAL` | SPI MISO domain pin on Mega. |
| D51 | LCD | `LCD_MOSI` | SPI MOSI. |
| D52 | LCD | `LCD_CLK` | SPI SCK. |
| D53 | LCD | `LCD_CS` | SPI SS/CS domain pin. |
| A8 | pressure sensor | `PIN_PRESSURE_SENSOR` | Feature-gated, disabled by default. |

## Conflict findings

- **Confirmed hard conflict**: Mega D49 reused by LCD reset and `PIN_BTN_SELECT`.
- **Potentially unsafe shared usage**: SPI-domain proximity on UNO/Nano
  (D10–D13) and Mega (50–53) is not guarded by compile-time policy when
  optional peripherals evolve.
- **Missing assignments**: UNO/Nano UI inputs intentionally unassigned
  (`0xFF`), but no compile-time policy enforces “must remain unallocated”
  when UI feature flags are enabled.

## Remapping strategy

1. **Immediate**: move Mega `PIN_BTN_SELECT` from D49 to free non-SPI pin
   (e.g., D33 as documented) and keep `LCD_RESET=49`.
2. Add compile-time pin uniqueness checks (`static_assert`) for every enabled
   peripheral group per board profile.
3. Add feature-aware reserved-pin policy:
   - If ST7920 enabled, reserve SPI + LCD control pins.
   - If UI feature disabled, force UI pins to `0xFF` to prevent accidental coupling.
4. Generate HARDWARE.md pin tables from `pins_*.h` to eliminate doc/code drift.

---

## E) Refactor Plan (Ordered)

### Immediate fixes (system break / correctness first)

1. Resolve Mega D49 conflict in `include/pins_mega2560.h` and synchronize `HARDWARE.md`.
2. Implement switch debounce layer for run/hold + section switches (input service with stable-state + timestamp).
3. Add telemetry frame budgeting: send bounded number of frames per loop and carry remainder to next cycle.
4. Add compile-time pin uniqueness/reserved-pin assertions per board.

### Structural fixes (architecture)

1. Introduce `TimeSourceAdapter` and remove direct `millis()` calls from
   `FlowSensor`/`WheelSensor` domain logic.
2. Split main loop into explicit phases/services (`InputService`,
   `ControlService`, `OutputService`, `TelemetryService`) with fixed execution
   budget.
3. Add bounded event queue for operator/menu events and serial command parser
   outputs.
4. Centralize pin policy in one generated artifact (or one header + codegen
   docs).

### Optimization / cleanliness

1. Consolidate repeated math helpers (`sanitizeNonNegativeFinite`, clamp
   helpers) into shared utility header.
2. Remove or guard unused protocol constants and placeholder pin declarations by
   feature flags.
3. Add static analysis checks for dead code and include hygiene.

---

## F) Recommended Improvements

### Better abstractions

- Add `TimeSourceAdapter` and `TelemetrySinkAdapter` to isolate Arduino runtime and serial transport from domain logic.
- Promote a dedicated `InputSnapshot` struct populated once per loop; pass immutable snapshot into control stage.

### Event system improvements

- Introduce ring buffer (`N=8..16`) for menu/events with explicit overflow counter and `RS:`/fault telemetry.
- Parse serial commands into typed events only; no immediate state mutation in parser path.

### Pin mapping redesign suggestions

- Enforce per-board `PinPolicy` struct with:
  - `required_interrupt_pins`
  - `reserved_spi_pins`
  - `optional_feature_pins`
  - compile-time uniqueness matrix
- Use one canonical source (headers) to auto-render docs pin table.

### Optional HAL improvements

- Keep current adapter interfaces, but add:
  - `DebouncedDigitalInputAdapter`
  - `MonotonicClockAdapter`
  - `TelemetryBudgetAdapter` (or scheduler service)
- This preserves frozen high-level behavior while improving determinism.

### Suggested testing strategy (unit + HIL)

- **Unit (host/sim)**:
  - Flow/wheel conversion edge tests (0 pulses, timeout, max clamp, invalid calibration).
  - FlowController closed-loop duty tests (ramp limits, no-flow fallback timer, stop semantics).
  - Menu state machine transition matrix and event overflow behavior.
- **HIL (board)**:
  - Pin conflict smoke test per board profile.
  - ISR pulse injection at multiple rates; verify stale/timeout fault thresholds.
  - Serial saturation scenario (max telemetry + command ingress) with deterministic latency assertions.
  - Switch bounce injection (relay-safe behavior, no chatter).

## Priority enforcement

- Applied audit priority: **hardware correctness > firmware stability > code style**.
