# ARCHITECTURE.md

Spray Controller Architecture

## Scalable Module Boundary Diagram

```text
+--------------------------------------------------------------------------------+
|                 Control Core (deterministic, platform-agnostic)                |
|--------------------------------------------------------------------------------|
| SystemState | SectionManager | FlowController | CoverageAccumulator            |
| FlowSensor  | WheelSensor    | PressureSensor | RunHoldSwitch | PumpControl    |
| Telemetry ordering / stable IDs | Operator menu state / reset semantics        |
+----------------------------------------+---------------------------------------+
                                         |
                                         | depends only on frozen interfaces
                                         v
+--------------------------------------------------------------------------------+
|                  Frozen Adapter Boundary (single seam for scale-out)           |
|--------------------------------------------------------------------------------|
| DigitalInputAdapter  | DigitalOutputAdapter | AnalogInputAdapter               |
| PwmOutputAdapter     | PulseCounterAdapter  | SectionHardwareAdapter           |
+----------------------------------------+---------------------------------------+
                                         |
                     +-------------------+-------------------+
                     |                                       |
                     v                                       v
+----------------------------------------+   +-----------------------------------+
| Arduino Adapter Implementations        |   | Bridge / Remote-MCU Implementations|
|----------------------------------------|   |-----------------------------------|
| ArduinoActiveLowInput                  |   | Section command/status transport  |
| ArduinoActiveHighOutput                |   | Sensor sample relay               |
| ArduinoAnalogInput                     |   | Pump / actuator command relay     |
| ArduinoPwmOutput                       |   | Heartbeat / fault propagation     |
| ArduinoInterruptPulseCounter           |   | Deterministic framing + retries   |
| ArduinoSectionHardwareAdapter          |   | Same frozen interfaces preserved  |
+----------------------------------------+   +-----------------------------------+
```

## Dependency Direction

- `Control Core` may depend only on frozen abstractions in `include/interfaces.h`.
- `Frozen Adapter Boundary` owns the only allowed I/O seam between control logic
  and any concrete board or bridge implementation.
- `Arduino Adapter Implementations` are the current single-MCU realization of
  that seam for the existing board build.
- `Bridge / Remote-MCU Implementations` are future peer implementations that may
  move section, sensor, or actuator I/O off-board without changing control-core
  behavior.
- Rule: dependencies point inward through interfaces only; control-core modules
  must not include raw Arduino GPIO APIs, pin maps, serial transport framing, or
  board-policy branches.

## Module Responsibilities

|Boundary|Modules|Primary responsibility|Must not depend on|
|---|---|---|---|
|Control Core|`SystemState`, `SectionManager`, `FlowController`, `CoverageAccumulator`, `FlowSensor`, `WheelSensor`, `PressureSensor`, `RunHoldSwitch`, `PumpControl`, operator-menu logic|Deterministic state transitions, control law, sensor interpretation, coverage math, and protocol-ready values|`digitalRead`, `digitalWrite`, `analogRead`, `analogWrite`, ISR wiring, concrete transport framing, board pin symbols|
|Frozen Interfaces|`DigitalInputAdapter`, `DigitalOutputAdapter`, `AnalogInputAdapter`, `PwmOutputAdapter`, `PulseCounterAdapter`, `SectionHardwareAdapter`|Stable contract for inputs, outputs, counters, and section access regardless of whether I/O is local or bridged|Control policy, board calibration rules, protocol serialization policy|
|Arduino Adapters|`ArduinoActiveLowInput`, `ArduinoActiveHighOutput`, `ArduinoAnalogInput`, `ArduinoPwmOutput`, `ArduinoInterruptPulseCounter`, `ArduinoSectionHardwareAdapter`|Translate frozen interface calls into direct MCU I/O and pin setup for the current hardware|Control decisions, telemetry ordering rules, section/pump business logic|
|Bridge Transport|Future bridge-backed implementations of the same adapter interfaces|Proxy deterministic reads/writes to another MCU or process while preserving frozen contracts and field ordering|Edits to control-core algorithms or protocol contracts|

## Multi-MCU Bridge-Ready Seams

### 1. Control Core ↔ Frozen Adapter Boundary

- Loop contract stays synchronous and deterministic: adapters provide sampled
  state first, control logic computes once, then command writes are emitted.
- Section iteration is keyed by stable `section_id` values from
  `kSectionDescriptors`, not by per-section branches.
- Sensor publication is keyed by stable identifiers and field order in
  `include/protocol.h`, so local and bridged providers preserve the same output
  contract.

### 2. Frozen Adapter Boundary ↔ Concrete Implementation

- Today the concrete implementation is Arduino-specific and lives entirely in
  adapter classes.
- A future bridge-backed implementation may satisfy the same interfaces over
  UART, CAN, I2C, or another deterministic link.
- Latency hiding, retries, and remote fault propagation belong inside the
  concrete implementation, not in `FlowController`, `SectionManager`, or other
  control-core modules.

### 3. Transport ↔ Remote Node

- Remote nodes may own GPIO, pulse counting, or sensor sampling, but must expose
  results in the same units and command semantics expected by the frozen
  interfaces.
- Bridge framing must preserve deterministic ordering for section commands,
  switch state, and sensor values so telemetry remains backward compatible.
- Faults detected remotely must surface through the same adapter methods used by
  the single-MCU build so the control core keeps one fault-handling path.

## SEE / THINK / DO Ownership

|Stage|Owned by|Current implementation|Scaling rule|
|---|---|---|---|
|SEE|Adapters + sensor wrappers|Arduino adapters feed `FlowSensor`, `WheelSensor`, `PressureSensor`, and `RunHoldSwitch`|Any local or bridged provider must preserve the same adapter contract, units, and ordering|
|THINK|Control core|`SystemState`, `SectionManager`, `FlowController`, `CoverageAccumulator`, operator menu|No GPIO, interrupt, or transport knowledge is allowed in this stage|
|DO|Control core issues intent through adapters|`PumpControl` and `SectionHardwareAdapter` send deterministic actuator intent|Actuator delivery may move off-board, but command semantics and ordering remain unchanged|

## Scaling Rules

- Section-count growth is handled by configuration and bounded descriptor-driven
  iteration, not by new per-section control branches.
- Deterministic telemetry field order and stable section/sensor IDs remain fixed
  even if sensing or actuation migrates behind a bridge.
- New boards or remote I/O nodes scale in by implementing existing adapter
  interfaces first; control-core APIs and protocol contracts stay frozen.
