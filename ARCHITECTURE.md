# ARCHITECTURE.md

Spray Controller Architecture

## Scalable Module Boundary Diagram

```text
+-----------------------------------------------------------------------+
|                        Control Core (platform-agnostic)               |
|-----------------------------------------------------------------------|
| SystemState | SectionManager | FlowController | CoverageAccumulator    |
| FlowSensor  | WheelSensor    | PressureSensor | RunHoldSwitch          |
| PumpControl | Protocol field ordering + deterministic ID mapping       |
+-----------------------------------+-----------------------------------+
                                    |
                                    | depends only on frozen interfaces
                                    v
+-----------------------------------------------------------------------+
|                      Adapter Boundary (single-MCU today)              |
|-----------------------------------------------------------------------|
| DigitalInputAdapter  | DigitalOutputAdapter | AnalogInputAdapter       |
| PwmOutputAdapter     | PulseCounterAdapter  | SectionHardwareAdapter   |
+-----------------------------------+-----------------------------------+
                                    |
                                    | implemented by board-specific or
                                    | bridge-backed transports
                                    v
+-----------------------------------+-----------------------------------+
| Arduino Adapters                  | Bridge / Remote MCU Transport     |
|-----------------------------------|-----------------------------------|
| ArduinoActiveLowInput             | Section command/status link       |
| ArduinoActiveHighOutput           | Sensor sample relay               |
| ArduinoAnalogInput                | Pump command relay                |
| ArduinoPwmOutput                  | Heartbeat / fault propagation     |
| ArduinoInterruptPulseCounter      | Deterministic message framing     |
| ArduinoSectionHardwareAdapter     | (future seam; same interfaces)    |
+-----------------------------------+-----------------------------------+
```

## Dependency Direction

- `Control Core` depends on frozen abstractions in `include/interfaces.h` only.
- `Adapter Boundary` owns all board-facing reads/writes and pin-mode setup.
- `Arduino Adapters` are the current concrete implementation of the adapter
  interfaces for the single-board build.
- `Bridge / Remote MCU Transport` is the future replacement/peer implementation
  point for distributed I/O without changing control-core logic.
- Dependency rule: outer layers may depend inward only through frozen
  interfaces; the control core must not include raw Arduino GPIO access.

## Module Responsibilities

|Boundary|Modules|Primary responsibility|Must not depend on|
|---|---|---|---|
|Control Core|`SystemState`, `SectionManager`, `FlowController`, `CoverageAccumulator`, `FlowSensor`, `WheelSensor`, `PressureSensor`, `RunHoldSwitch`, `PumpControl`|Deterministic state, control law, section aggregation, coverage math, protocol-ready values|Raw pins, `digitalRead`, `digitalWrite`, `analogRead`, `analogWrite`, board pin maps|
|Frozen Interfaces|`DigitalInputAdapter`, `DigitalOutputAdapter`, `AnalogInputAdapter`, `PwmOutputAdapter`, `PulseCounterAdapter`, `SectionHardwareAdapter`|Stable seam between logic and hardware/transport implementations|Board-specific policy or transport framing details|
|Board Adapters|`ArduinoActiveLowInput`, `ArduinoActiveHighOutput`, `ArduinoAnalogInput`, `ArduinoPwmOutput`, `ArduinoInterruptPulseCounter`, `ArduinoSectionHardwareAdapter`|Translate interface calls into direct MCU I/O for the current board|Control decisions, protocol policy|
|Transport Seam|Future bridge-backed implementations of the same adapters|Allow section, sensor, and actuator I/O to move to another MCU/process while preserving frozen control logic|Direct edits to control-core algorithms|

## Multi-MCU Bridge-Ready Seams

### 1. Control Core ↔ Adapter Boundary

- Contract is synchronous and deterministic: each loop reads adapter state, then
  computes outputs, then writes adapter commands.
- Section access is keyed by stable `section_id` values from
  `kSectionDescriptors`, not by hard-coded loop branches.
- Sensor handling is keyed by stable telemetry/sensor identifiers in
  `include/protocol.h`, so local and remote providers preserve field ordering.

### 2. Adapter Boundary ↔ Transport

- A bridge implementation may satisfy `SectionHardwareAdapter` and the sensor /
  actuator adapters by forwarding reads and writes over UART/CAN/I2C/other
  deterministic framing.
- Transport latency handling belongs in the adapter implementation, not in
  `FlowController`, `SectionManager`, or protocol formatting logic.
- Fault propagation from a remote node must be surfaced back through the same
  adapter methods so the control core retains one fault-handling path.

## Loop Ownership (SEE / THINK / DO)

|Stage|Owned by|Current implementation|Scalable rule|
|---|---|---|---|
|SEE|Adapters + sensor wrappers|Arduino adapters feed `FlowSensor`, `WheelSensor`, `PressureSensor`, `RunHoldSwitch`|Any local or bridged implementation must preserve the same adapter contract and units|
|THINK|Control core|`SystemState`, `SectionManager`, `FlowController`, `CoverageAccumulator`|No transport or GPIO knowledge allowed in this stage|
|DO|Control core issues commands through adapters|`PumpControl` + `SectionHardwareAdapter` write actuator intent|Actuator delivery may move off-board, but command semantics remain unchanged|

## Scaling Rules

- Section-count growth is handled by configuration/descriptors and bounded
  static containers, not by new per-section control branches.
- Deterministic protocol field order and telemetry IDs remain fixed even if the
  hardware implementation moves behind a bridge.
- New boards or remote I/O nodes must plug in by implementing existing adapter
  interfaces first; control-core APIs stay frozen.
