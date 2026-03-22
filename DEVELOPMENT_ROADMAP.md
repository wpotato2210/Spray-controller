# Spray Controller Deterministic Roadmap (Arduino Nano)

## P0 - Baseline Audit Freeze

**Objective:** Lock current behavior before expansion.

### Deterministic tasks

1. Freeze current module contracts in `include/interfaces.h`.
2. Confirm `src/main.cpp` loop cadence against

`config.yaml:timing.loop_interval_ms`.

3. Record current assumptions: single flow sensor, single pump PWM, and

3 binary sections.

### Deliverables (done = all complete)

- Audit note in `DEV_NOTES.md` with current loop path and ownership.
- Interface checklist matching source files.
- No interface signature drift from baseline.

## P1 - Pin + Config Finalization

**Objective:** Make runtime constants and pins authoritative and validated.

### Deterministic tasks (2)

1. Finalize `pins.yaml` for a Nano-safe map.
2. Finalize `config.yaml` keys for gains, limits, and timing.
3. Add deterministic YAML validation (`validation.yaml`) in CI or manual

checks.

4. Define one canonical loop interval (for example, 50 ms) and enforce it in

firmware constants.

### Deliverables (done = all complete) (2)

- Final `pins.yaml` and `config.yaml` schema with no TODO fields.
- Validation pass output with no missing or extra keys.
- `CONFIGURATION.md` and `HARDWARE.md` updated to exact values.

## P2 - Core Control Loop Integration

**Objective:** Complete run-ready SEE -> THINK -> DO behavior.

### Deterministic tasks (3)

1. Integrate run/hold gating so HOLD forces pump minimum and output-safe

section state.

2. Integrate global flow controller with active boom width from section

manager.

3. Add a loop determinism guard (elapsed-time check plus fixed order).
4. Publish a compact serial status frame each loop interval.

### Deliverables (done = all complete) (3)

- `src/main.cpp` executes a stable deterministic cycle.
- `src/flow_controller.cpp`, `src/pump_control.cpp`, and

`src/section_manager.cpp` behavior aligned to spec.

- Protocol output documented and reproducible.

## P3 - Sensor Robustness

**Objective:** Stabilize wheel and flow measurements and optional pressure
telemetry.

### Deterministic tasks (4)

1. Add pulse timeout and zero-speed handling for the wheel sensor.
2. Add flow plausibility clamp and stale-read fallback.
3. Add optional pressure sensor abstraction (`readPressure()`), telemetry only

(no control coupling).

4. Define sensor fault flags for protocol and debug output.

### Deliverables (done = all complete) (4)

- `flow_sensor` and `wheel_sensor` produce bounded values under no-pulse and

burst conditions.

- Optional pressure module is compile-time guarded and disabled by default.
- Fault bitfield and status text added to telemetry definition.

## P4 - Operator Interface

**Objective:** Deliver usable field UI with safety confirmations.

### Deterministic tasks (5)

1. Implement menu navigation states.
2. Add live preview fields (speed, flow, duty, active sections).
3. Add distance and area accumulators based on wheel data and active width.
4. Add reset confirmation workflow for counters and calibration.

### Deliverables (done = all complete) (5)

- Operator menu state machine documented and tested.
- Live preview updates at fixed interval.
- Distance and area reset requires explicit confirm action.

## P5 - Scalability Preparation

**Objective:** Prepare for more sections and sensors without logic rewrites.

### Deterministic tasks (6)

1. Replace hard-coded section arrays with config-driven descriptors.
2. Introduce compile-time max constants (`MAX_SECTIONS`, `MAX_SENSORS`).
3. Isolate hardware adapters from control logic for a future multi-MCU bridge.
4. Add deterministic ID mapping for section and sensor telemetry.

### Deliverables (done = all complete) (6)

- Section count changes require config updates only.
- Interfaces remain backward compatible for the current 3-section build.
- `ARCHITECTURE.md` updated with scalable module boundaries.

## P6 - Calibration Workflow

**Objective:** Make calibration repeatable with optional manual override.

### Deterministic tasks (7)

1. Define flow calibration sequence (pulses/L or factor).
2. Define wheel calibration sequence (distance pulses).
3. Store and reload calibration constants with defaults and a checksum or

validity marker.

4. Add operator override flag (use defaults versus calibrated values).

### Deliverables (done = all complete) (7)

- `CALIBRATION.md` converted from placeholder to executable steps.
- Calibration constants persisted and sanity-checked at boot.
- Override path tested and documented.

## P7 - Documentation Freeze

**Objective:** Freeze docs for Codex build reproducibility.

### Deterministic tasks (8)

1. Align `ARCHITECTURE.md`, `PROTOCOLS.md`, `USAGE.md`, and `TESTING.md` with

implemented firmware.

2. Replace placeholder diagrams in `DIAGRAMS.md` with final SEE/THINK/DO,

wiring, and state flow.

3. Mark frozen interface version and date.

### Deliverables (done = all complete) (8)

- Doc set is internally consistent (pins, fields, units, and states).
- No placeholder markers remain in core docs.
- Frozen-doc tag recorded in `CHANGELOG.md`.

## P8 - Release Readiness

**Objective:** Produce stable tagged firmware release.

### Deterministic tasks (9)

1. Run full bench validation matrix (run/hold, section toggles, disconnect

tests, and min/max speed).

2. Run YAML validation and config compatibility checks.
3. Bump firmware version, tag release, and update `CHANGELOG.md` with exact

date.

4. Archive known limitations.

### Deliverables (done = all complete) (9)

- Release candidate passes all mandatory tests.
- Git tag, changelog, and config versions are aligned.
- Reproducible build and flash instructions verified in `INSTALL.md`.

## P9 - Optional Enhancements

**Objective:** Non-blocking extensions after stable release.

### Deterministic tasks (10)

1. Multi-MCU transport abstraction (UART/CAN bridge).
2. Advanced UI (preset profiles, guided calibration).
3. Remote logging output channel.
4. GPS-driven area and speed fallback logic.

### Deliverables (done = all complete) (10)

- Optional feature-branch specs with acceptance criteria.
- No regression to baseline single-Nano firmware path.

## Phase Gates (Execution Order)

1. Must complete in order: P0 -> P1 -> P2 -> P3 -> P4 -> P5 -> P6 -> P7 ->

P8.

2. P9 starts only after P8 tag exists.
3. Gate rule: each phase closes only when all listed deliverables are present

in repo and referenced from `CHANGELOG.md`.
