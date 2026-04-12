# Full Engineering Audit + Refactor Recommendations (2026-04-12)

## A) Executive Summary

### Top 5 systemic risks

1. **SRAM pressure on UNO/Nano from full-framebuffer LCD mode (`_F_`)**
   can destabilize runtime when combined with telemetry buffers, event queue,
   and stack usage.
2. **Pin-policy compile checks are partial** (UI + selected roles only), so some future pin conflicts can compile without detection.
3. **Documentation/build dependency drift** exists: docs still claim “no additional libraries,” but firmware hard-depends on U8g2 for display.
4. **Single-loop cooperative scheduler couples all timing domains**
   (serial ingress, control, display render, telemetry), increasing jitter
   sensitivity under load.
5. **ISR pulse timestamping uses `millis()` and split reads**
   (`count` then `last_pulse_ms`) which is adequate for low rates but can
   produce timing skew at higher pulse frequencies.

### Top 5 engineering weaknesses

1. **Display backend is hard-wired to ST7920 HW SPI**,
   limiting portability to I2C/SPI LCD variants despite hardware audit scope
   requiring variant support planning.
2. **Menu/event queue processes only one event per loop**, so bursts can lag and overflow under sustained serial command traffic.
3. **No explicit fault-latching/recovery policy layer**; faults are computed
   ad hoc and reported, but not centrally mediated for degraded-mode behavior.
4. **Runtime object graph is global in `.ino` namespace**, which increases implicit state coupling and reduces test seam clarity.
5. **Board support statements are inconsistent across docs**, risking wrong integration assumptions during bring-up.

---

## B) Issue Table (MANDATORY)

| Severity | Area | Issue | Evidence | Fix Recommendation |
| --- | --- | --- | --- | --- |
| CRITICAL | Firmware + Memory | UNO/Nano SRAM risk from `U8G2_ST7920_128X64_F_HW_SPI` full framebuffer mode | Display uses `_F_` constructor and multiple local line buffers during render; AVR UNO/Nano SRAM budget is tight. | Switch to page buffer mode (`_1_` or `_2_`), and add a compile-time memory profile note per board. |
| HIGH | Hardware Integration | Pin conflict prevention is incomplete (not all functional pins checked against LCD/SPI reserved set) | `pin_policy_guards.h` checks UI pins against LCD-reserved pins, but does not assert all assigned role pins vs LCD pins. | Extend static asserts to validate **all assigned pins** (`kAssignedRolePins`) against reserved LCD/SPI set when LCD enabled. |
| HIGH | Build/Config | Dependency hygiene mismatch: docs claim no extra libraries while code requires U8g2 | `display.cpp` includes `<U8g2lib.h>`; README/INSTALL state no additional libraries. | Update docs + install path to require U8g2 (pin a tested version/range) and fail CI if library missing. |
| HIGH | Architecture/Timing | Cooperative loop handles parser, menu, control, display, telemetry in one path | `loop()` performs all phases serially with fixed cadence guard. | Introduce explicit per-phase time budgets and a lightweight scheduler with overrun counters. |
| MEDIUM | ISR/Timing | Pulse counter snapshot is not atomic across `readCount()` + `readLastPulseMs()` pair | Sensors call `readCount()` and `readLastPulseMs()` separately; ISR can update between reads. | Add `readSnapshot(count,last_ms)` API protected by one interrupt lock. |
| MEDIUM | Event System | Menu event dequeue budget is one event/loop; overflow only indirectly mitigated | `processOperatorEventQueue` dequeues once per loop; queue capacity is 8. | Process up to N events/loop (e.g., 2–4), publish queue depth telemetry, keep overflow reporting. |
| MEDIUM | Peripheral Compatibility | LCD implementation only supports ST7920 HW SPI mapping; no HAL path for I2C variant | `display.cpp` and `pin_map_lcd.h` are hard-coded for ST7920 SPI pins. | Add compile-time display backend selection (`ST7920_SPI`, `I2C_1602`, `NONE`) with adapter interface. |
| MEDIUM | Hardware/Electrical | Input assumptions rely on internal pull-ups without per-input electrical contract checks | Switches/sensors configured as `INPUT_PULLUP`; no runtime detect for inverted/open-drain variants. | Document required electrical interface (voltage, pull-up value, polarity), and add inversion/config flags per input class. |
| LOW | Code Quality | Repeated linear scans on section descriptors in multiple paths | `readSwitch`, `writeSection`, and section switch-state lookups iterate over descriptors each call. | Cache index by section id or constrain to contiguous IDs with direct index access + static_assert. |
| LOW | State Logic | Fault bits are computed on-demand; no centralized fault manager for latching/clear policy | `getStatusFaultBitfield()` assembles transient status each publish. | Add `FaultManager` module with latching policies, debounce windows, and explicit recovery criteria. |

---

## C) Architecture Diagram (TEXT ONLY)

```text
[Board Pin Map]
  pins_<board>.h + pin_map_lcd.h + pin_policy_guards.h
            |
            v
[HAL Adapters]
  ArduinoActiveLowInput / ActiveHighOutput / PwmOutput
  ArduinoInterruptPulseCounter / ArduinoAnalogInput / MillisTimeSource
            |
            v
[Input + Sensing]
  RunHoldSwitch
  SectionHardwareAdapter -> SectionManager
  FlowSensor / WheelSensor / PressureSensor
            |
            v
[Control]
  FlowController (PI + slew-rate clamp + no-flow fallback)
  CoverageAccumulator
            |
            v
[Application Orchestration in loop()]
  processOperatorCommand -> event queue -> OperatorMenuStateMachine
  runInputPhase -> runControlPhase -> runOutputPhase -> runTelemetryPhase
            |
            +--> Pump PWM + section outputs
            +--> U8g2 display render
            +--> Serial telemetry/protocol
```

### Bottlenecks / unsafe coupling

- `loop()` is a single cooperative execution lane: display transfer and serial workload compete with control cadence.
- Global object instances in `.ino` share mutable state implicitly across phases.
- Telemetry and command ingress share one serial transport and timing budget.

---

## D) Pin & Hardware Conflict Audit (CRITICAL)

## Full pin usage table

### Arduino UNO

| Pin | Module | Function | Board dependency |
| --- | --- | --- | --- |
| D2 | wheel sensor | `PIN_WHEEL_SENSOR` | External interrupt pin (UNO valid). |
| D3 | flow sensor | `PIN_FLOW_SENSOR` | External interrupt pin (UNO valid). |
| D4 | section input | `PIN_SECTION_SW_3` | Input pull-up. |
| D7 | section output | `PIN_SECTION_RELAY_2` | Output. |
| D8 | LCD | `LCD_RESET` | ST7920 reset. |
| D9 | pump | `PIN_PUMP_PWM` | PWM output. |
| D10 | LCD | `LCD_CS` | SPI CS domain. |
| D11 | LCD | `LCD_MOSI` | SPI MOSI domain. |
| D12 | section output | `PIN_SECTION_RELAY_1` | Also SPI MISO pin on AVR header. |
| D13 | LCD | `LCD_CLK` | SPI SCK domain. |
| A0 | pressure | `PIN_PRESSURE_SENSOR` | Optional analog input (feature-gated). |
| A1 | section output | `PIN_SECTION_RELAY_3` | Analog pin used as digital output. |
| A2 | indicator | `PIN_LED_SECTION_1` | Output. |
| A3 | run/hold | `PIN_RUN_HOLD` | Input pull-up. |
| A4 | section input | `PIN_SECTION_SW_1` | Input pull-up (I2C SDA domain). |
| A5 | section input | `PIN_SECTION_SW_2` | Input pull-up (I2C SCL domain). |
| 0xFF | UI pins | encoder/buttons | Unassigned placeholders. |

### Arduino Nano

| Pin | Module | Function | Board dependency |
| --- | --- | --- | --- |
| D2 | wheel sensor | `PIN_WHEEL_SENSOR` | External interrupt pin (Nano valid). |
| D3 | flow sensor | `PIN_FLOW_SENSOR` | External interrupt pin (Nano valid). |
| D4 | section input | `PIN_SECTION_SW_3` | Input pull-up. |
| D5 | encoder placeholder | `PIN_ENCODER_CLK` | Assigned but currently unused in logic. |
| D6 | encoder placeholder | `PIN_ENCODER_DT` | Assigned but currently unused in logic. |
| D7 | section output | `PIN_SECTION_RELAY_2` | Output. |
| D8 | LCD | `LCD_RESET` | ST7920 reset. |
| D9 | pump | `PIN_PUMP_PWM` | PWM output. |
| D10 | LCD | `LCD_CS` | SPI CS domain. |
| D11 | LCD | `LCD_MOSI` | SPI MOSI domain. |
| D12 | section output | `PIN_SECTION_RELAY_1` | Also SPI MISO pin on AVR header. |
| D13 | LCD | `LCD_CLK` | SPI SCK domain. |
| A0 | pressure | `PIN_PRESSURE_SENSOR` | Optional analog input (feature-gated). |
| A1 | section output | `PIN_SECTION_RELAY_3` | Analog pin used as digital output. |
| A2 | indicator | `PIN_LED_SECTION_1` | Output. |
| A3 | run/hold | `PIN_RUN_HOLD` | Input pull-up. |
| A4 | section input | `PIN_SECTION_SW_1` | Input pull-up (I2C SDA domain). |
| A5 | section input | `PIN_SECTION_SW_2` | Input pull-up (I2C SCL domain). |

### Arduino Mega 2560

| Pin | Module | Function | Board dependency |
| --- | --- | --- | --- |
| D18 | wheel sensor | `PIN_WHEEL_SENSOR` | External interrupt-capable. |
| D19 | flow sensor | `PIN_FLOW_SENSOR` | External interrupt-capable. |
| D22 | section output | `PIN_SECTION_RELAY_1` | Output. |
| D23 | section output | `PIN_SECTION_RELAY_2` | Output. |
| D24 | section output | `PIN_SECTION_RELAY_3` | Output. |
| D25 | indicator | `PIN_LED_SECTION_1` | Output. |
| D26 | run/hold | `PIN_RUN_HOLD` | Input pull-up. |
| D27 | section input | `PIN_SECTION_SW_1` | Input pull-up. |
| D28 | section input | `PIN_SECTION_SW_2` | Input pull-up. |
| D29 | section input | `PIN_SECTION_SW_3` | Input pull-up. |
| D30 | encoder | `PIN_ENCODER_CLK` | Assigned (UI expansion). |
| D31 | encoder | `PIN_ENCODER_DT` | Assigned (UI expansion). |
| D32 | encoder/button | `PIN_ENCODER_SW` | Assigned (UI expansion). |
| D33 | button | `PIN_BTN_UP` | Assigned (UI expansion). |
| D34 | button | `PIN_BTN_DOWN` | Assigned (UI expansion). |
| D35 | button | `PIN_BTN_CAL` | Assigned (UI expansion). |
| D36 | button | `PIN_BTN_SELECT` | Assigned (UI expansion). |
| D37 | button | `PIN_BTN_AUTO_MANUAL` | Assigned (UI expansion). |
| D44 | pump | `PIN_PUMP_PWM` | PWM output. |
| D49 | LCD | `LCD_RESET` | ST7920 reset. |
| D51 | LCD | `LCD_MOSI` | SPI MOSI. |
| D52 | LCD | `LCD_CLK` | SPI SCK. |
| D53 | LCD | `LCD_CS` | SPI SS/CS domain. |
| A8 | pressure | `PIN_PRESSURE_SENSOR` | Optional analog input. |

## Conflict findings

- **No active hard pin conflicts detected in current board maps**.
- **Policy gap:** compile-time reserved-pin checks protect UI group, but not every assigned functional role vs LCD/SPI reservations.
- **UNO/Nano integration caveat:** A4/A5 are consumed by section switches, so I2C peripherals are not drop-in without remap.

## Remapping strategy (if/when conflicts arise)

1. Keep interrupt sensors on dedicated external interrupt pins per board (`2/3` on UNO/Nano, `18/19` on Mega baseline).
2. Reserve LCD SPI domain as immutable when ST7920 backend is enabled.
3. Move optional UI roles before touching core safety/control pins:
   - Mega: preferred spare range `38..43,45..48`.
   - UNO/Nano: prefer feature-gating (unassigned `0xFF`) rather than forced multiplexing.
4. Add board-specific `static_assert` matrix validating uniqueness + reserved-domain exclusions for **all enabled roles**.

---

## E) Refactor Plan (Ordered)

### Immediate fixes (breaks system / correctness)

1. **Change LCD constructor to page-buffer mode** on AVR (`_1_` or `_2_`) and verify runtime stability.
2. **Expand compile-time pin policy guards** to cover all assigned roles against reserved LCD/SPI pins.
3. **Fix docs/install dependency statements** to explicitly require U8g2.

### Structural fixes (architecture)

1. Introduce `SchedulerStats` and phase budgets
   (`input/control/output/telemetry/ui`) with overrun counters.
2. Add atomic pulse snapshot API in `PulseCounterAdapter`
   to remove split-read race/skew.
3. Add display HAL selection and isolate ST7920 implementation
   behind `DisplayAdapter`.
4. Add `FaultManager` module for latching, escalation, and clear criteria.

### Optimization (performance/cleanliness)

1. Increase event dequeue budget (bounded) and expose queue depth telemetry.
2. Replace repeated section descriptor linear searches with direct indexed
   access where IDs are contiguous.
3. Add CI compile matrix (`uno`, `nano`, `mega2560`) and one
   docs-consistency validation step.

---

## F) Recommended Improvements

### Better abstractions

- Introduce explicit interfaces for:
  - `DisplayAdapter` (render contract independent of U8g2 backend)
  - `FaultManager` (aggregated fault policy)
  - `Scheduler` (phase budget + instrumentation)

### Event system improvements

- Keep ring buffer but process `min(queue_depth, kPerLoopBudget)` events each loop.
- Emit queue depth + overflow counter in telemetry to support field diagnosis.

### Pin mapping redesign suggestions

- Define one board policy struct per target:
  - interrupt-required pins
  - reserved buses (SPI/I2C/UART)
  - optional feature pools
- Enforce via compile-time assertions and generate hardware tables from headers.

### Optional HAL improvements

- Add `readSnapshot()` to pulse counters.
- Add input polarity config and optional software debouncer policy per input type.
- Add display backend selection macros and compile-time validation.

### Suggested testing strategy (unit + HIL)

- **Unit tests (host):**
  - Pulse snapshot consistency under simulated ISR interleaving.
  - FlowController behavior (slew clamp, fallback duty, reset semantics).
  - Queue overflow + multi-event dequeue fairness.
  - FaultManager latching/clear rules.
- **Hardware-in-loop:**
  - Long-duration SRAM soak test on UNO/Nano with display + telemetry enabled.
  - Burst serial command tests validating queue overflow telemetry and menu responsiveness.
  - Interrupt stress tests for flow/wheel pulse channels across low/high rates.
