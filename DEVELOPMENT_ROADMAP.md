# Spray Controller Deterministic Roadmap (Arduino Nano)

## Execution Rules

- Phase order is strict: P0 -> P1 -> P2 -> P3 -> P4 -> P5 -> P6 -> P7 -> P8.
- P9 starts only after P8 release tag exists.
- A phase closes only when all tasks, outputs, validation steps, and rollback notes
  are complete and closure is recorded in `CHANGELOG.md`.
- No task may start until all listed prerequisites are complete.

## P0 - Baseline Audit Freeze

**Objective:** Lock current behavior before expansion.

**Prerequisites:** None.

### P0 Inputs

- Current firmware entrypoint in `Spray-controller.ino`.
- Public contracts in `include/interfaces.h`.
- Current configuration assets: `config.yaml`, `pins.yaml`, `protocols.yaml`,
  `validation.yaml`.

### P0 Deterministic tasks

1. Inventory current control-loop entrypoints and execution order in
   `Spray-controller.ino`.
2. Freeze current public module contracts in `include/interfaces.h`.
3. Verify current loop cadence against `config.yaml:timing.loop_interval_ms`.
4. Record baseline assumptions: single flow sensor, single pump PWM, 3 binary
   sections.
5. Create an interface checklist that maps each frozen interface to its current
   implementation file.

### P0 Outputs

- Baseline loop-path audit in `DEV_NOTES.md`.
- Frozen interface checklist linked to implementation files.
- Baseline assumption record for control model, sensors, and sections.

### P0 Validation

- Confirm every interface in `include/interfaces.h` has an owning
  implementation file.
- Confirm roadmap file paths match repository paths.
- Confirm loop cadence source is the same in code and `config.yaml`.

### P0 Failure / Recovery

- If file-path or interface drift is found, stop phase closure and update the
  audit artifacts before P1 starts.
- If cadence source is duplicated, identify one canonical source before P1.

## P1 - Pin + Config Finalization

**Objective:** Make runtime constants and pins authoritative and validated.

**Prerequisites:** P0 outputs complete.

### P1 Inputs

- P0 baseline audit and frozen interfaces.
- `pins.yaml`, `config.yaml`, `validation.yaml`, `openspec.yaml`.

### P1 Deterministic tasks

1. Finalize `pins.yaml` for the Arduino Nano production map.
2. Finalize `config.yaml` keys for gains, limits, timing, and feature toggles.
3. Define one canonical loop interval and bind firmware constants to it.
4. Add deterministic YAML/config validation to the standard validation path.
5. Update `CONFIGURATION.md` and `HARDWARE.md` to exact finalized values.

### P1 Outputs

- Finalized pin map and runtime configuration schema.
- Deterministic validation command covering YAML and config completeness.
- Documentation synchronized to final pin/config values.

### P1 Validation

- Run YAML/schema/config validation with no missing, extra, or conflicting
  keys.
- Confirm firmware constants match finalized configuration values.
- Confirm docs match `pins.yaml` and `config.yaml` exactly.

### P1 Failure / Recovery

- If validation fails, revert to the last passing pin/config set before further
  firmware integration.
- If docs diverge from config, fix docs before phase closure.

## P2 - Core Control Loop Integration

**Objective:** Complete run-ready SEE -> THINK -> DO behavior.

**Prerequisites:** P1 outputs complete.

### P2 Inputs

- Finalized loop interval, pin map, and config values from P1.
- Frozen interfaces from P0.

### P2 Deterministic tasks

1. Integrate run/hold gating so HOLD forces pump minimum and safe section
   output state.
2. Integrate active boom width computation from the section manager.
3. Integrate global flow controller using active boom width and target rate.
4. Add a loop determinism guard: fixed execution order plus elapsed-time
   monitoring.
5. Publish one compact serial status frame per loop interval.
6. Add bench integration checkpoint for run/hold, section-output, and pump-PWM
   timing.

### P2 Outputs

- Stable deterministic control cycle in `Spray-controller.ino`.
- Control-path alignment across `src/flow_controller.cpp`,
  `src/pump_control.cpp`, and `src/section_manager.cpp`.
- Reproducible status-frame contract.

### P2 Validation

- Verify loop ordering is fixed and executes once per configured interval.
- Verify HOLD state clamps outputs to the defined safe behavior.
- Verify status frame is emitted at the loop interval with deterministic field
  order.
- Verify bench integration checkpoint passes for run/hold and output timing.

### P2 Failure / Recovery

- If loop overruns occur, block phase closure and record the failing path.
- If HOLD does not force safe outputs, revert the integration to the last known
  safe gating behavior.

## P3 - Sensor Robustness

**Objective:** Stabilize wheel and flow measurements and optional pressure
telemetry.

**Prerequisites:** P2 outputs complete.

### P3 Inputs

- Deterministic control loop and status telemetry from P2.
- Sensor interfaces and config limits from P0/P1.

### P3 Deterministic tasks

1. Add wheel pulse timeout handling.
2. Add explicit zero-speed output on stale wheel input.
3. Add flow plausibility clamp for out-of-range measurements.
4. Add stale-read fallback behavior for the flow sensor.
5. Add optional pressure abstraction through `readPressure()` for telemetry
   only.
6. Define and emit sensor fault flags in protocol and debug output.
7. Add hardware/software integration checkpoint for missing-pulse,
   disconnect, and burst-input cases.

### P3 Outputs

- Bounded wheel and flow sensor outputs under stale and burst conditions.
- Optional pressure telemetry module disabled by default and compile-time
  guarded.
- Fault-bit telemetry contract for sensor status.

### P3 Validation

- Verify wheel sensor returns deterministic zero-speed on timeout.
- Verify flow sensor clamps and stale fallback behavior under invalid input.
- Verify pressure telemetry cannot affect control output.
- Verify fault bits and status text map deterministically to sensor failures.
- Verify disconnect and burst-input integration checkpoint passes.

### P3 Failure / Recovery

- If a sensor path can drive control with stale data, disable that path and
  restore safe fallback behavior.
- If pressure telemetry couples into control, remove the coupling before phase
  closure.

## P4 - Operator Interface

**Objective:** Deliver usable field UI with safety confirmations.

**Prerequisites:** P3 outputs complete.

### P4 Inputs

- P3 fault telemetry and stable runtime metrics.
- Current operator input/output interfaces.

### P4 Deterministic tasks

1. Implement deterministic menu navigation states.
2. Implement live preview fields for speed, flow, duty, and active sections.
3. Implement distance accumulation from wheel data.
4. Implement area accumulation from wheel data and active width.
5. Implement reset-confirmation workflow for counters.
6. Implement reset-confirmation workflow for calibration data.
7. Add operator-interface integration checkpoint for preview refresh,
   confirmation flow, and fault visibility.

### P4 Outputs

- Documented operator menu state machine.
- Fixed-interval live preview payload.
- Confirmed-reset workflow for counters and calibration actions.

### P4 Validation

- Verify all menu transitions are deterministic and reachable.
- Verify preview updates at a fixed interval.
- Verify resets require explicit confirmation and produce deterministic events.
- Verify operator interface exposes active faults and safe runtime state.

### P4 Failure / Recovery

- If reset can occur without confirmation, block closure and revert to the last
  safe reset path.
- If preview timing degrades the main loop, reduce UI work or move it to a
  gated interval before closure.

## P5 - Scalability Preparation

**Objective:** Prepare for more sections and sensors without logic rewrites.

**Prerequisites:** P4 outputs complete.

### P5 Inputs

- Stable section control, telemetry, and operator workflows from P2-P4.
- Current board and adapter abstractions.

### P5 Deterministic tasks

1. Replace hard-coded section arrays with config-driven or descriptor-driven
   section definitions.
2. Introduce compile-time max constants for sections and sensors.
3. Enforce static bounds checks against max constants.
4. Isolate hardware adapters from control logic for future multi-MCU bridging.
5. Add deterministic section-ID and sensor-ID telemetry mapping.
6. Add compatibility checkpoint for the current 3-section build.

### P5 Outputs

- Descriptor-driven section topology.
- Compile-time scalability bounds.
- Stable backward-compatible telemetry IDs.
- Updated architecture boundary definition.

### P5 Validation

- Verify section-count changes do not require control-loop rewrites.
- Verify current 3-section behavior remains unchanged.
- Verify telemetry IDs stay stable across descriptor-count changes.
- Verify adapter isolation keeps raw hardware access out of control modules.

### P5 Failure / Recovery

- If descriptor changes break the 3-section build, restore the last compatible
  descriptor mapping before closure.
- If adapter isolation is incomplete, block multi-MCU preparation claims.

## P6 - Calibration Workflow

**Objective:** Make calibration repeatable with optional manual override.

**Prerequisites:** P5 outputs complete.

### P6 Inputs

- Stable operator workflows from P4.
- Stable storage and descriptor model from P5.

### P6 Deterministic tasks

1. Define the flow calibration sequence.
2. Define the wheel calibration sequence.
3. Implement operator entrypoints for both calibration sequences.
4. Store calibration constants with defaults plus checksum or validity marker.
5. Reload and validate calibration constants at boot.
6. Add operator override for defaults versus calibrated values.
7. Add failure detection for invalid, missing, or corrupt stored calibration.
8. Add calibration integration checkpoint covering run, save, reboot, reload,
   and override paths.

### P6 Outputs

- Executable calibration procedure in `CALIBRATION.md`.
- Persisted calibration constants with boot-time validation.
- Tested override path for default versus calibrated values.

### P6 Validation

- Verify each calibration workflow can be completed from the operator
  interface.
- Verify stored values survive reboot and fail closed on invalid checksum or
  marker.
- Verify override path selects the intended value source deterministically.
- Verify integration checkpoint passes for save/reload/override scenarios.

### P6 Failure / Recovery

- If calibration storage is invalid, fall back to defaults and emit a fault.
- If reload changes active values unexpectedly, block closure until the source
  selection path is deterministic.

## P7 - Documentation Freeze

**Objective:** Freeze core docs for build and validation reproducibility.

**Prerequisites:** P6 outputs complete.

### P7 Inputs

- Implemented firmware behavior through P6.
- Final protocol, calibration, and architecture artifacts.

### P7 Deterministic tasks

1. Align `ARCHITECTURE.md`, `PROTOCOLS.md`, `USAGE.md`, and `TESTING.md` with
   implemented firmware.
2. Replace placeholder diagrams in `DIAGRAMS.md` with final SEE/THINK/DO,
   wiring, and state flow.
3. Mark frozen interface version and date.
4. Add a documentation consistency checkpoint across pins, fields, units,
   states, and calibration steps.

### P7 Outputs

- Internally consistent core documentation set.
- Final diagrams for architecture, wiring, and state flow.
- Frozen interface version record.

### P7 Validation

- Verify no placeholder markers remain in core docs.
- Verify protocol fields, units, and state names match implementation.
- Verify documentation consistency checkpoint passes.
- Verify changelog records documentation freeze closure.

### P7 Failure / Recovery

- If docs conflict with implementation, update docs before closure.
- If freeze metadata is incomplete, block P7 closure.

## P8 - Release Readiness

**Objective:** Produce stable tagged firmware release.

**Prerequisites:** P7 outputs complete.

### P8 Inputs

- Frozen docs and validated implementation through P7.
- All deterministic validators and bench procedures.

### P8 Deterministic tasks

1. Run the full bench validation matrix: run/hold, section toggles,
   disconnect tests, min/max speed, calibration persistence, and override.
2. Run YAML, protocol, config, and phase-specific validation checks.
3. Verify reproducible build and flash instructions in `INSTALL.md`.
4. Bump firmware version and config version as required.
5. Update `CHANGELOG.md` with exact release date and gate evidence.
6. Create release tag.
7. Archive known limitations and open non-blocking follow-ups.

### P8 Outputs

- Release candidate with passing mandatory validation matrix.
- Aligned version, tag, changelog, and installation instructions.
- Archived known limitations.

### P8 Validation

- Verify all mandatory tests pass.
- Verify version identifiers are consistent across firmware, configs,
  documentation, and tag.
- Verify release build can be reproduced from repository instructions.

### P8 Failure / Recovery

- If any mandatory validation fails, do not tag; reopen the owning phase.
- If reproducible build fails, fix `INSTALL.md` or build inputs before release.
- If version alignment fails, block tagging until all versioned artifacts match.

## P9 - Optional Enhancements

**Objective:** Non-blocking extensions after stable release.

**Prerequisites:** P8 release tag exists.

### P9 Inputs

- Stable tagged release baseline.
- Archived known limitations and future-feature backlog.

### P9 Deterministic tasks

1. Define a transport abstraction for multi-MCU UART/CAN bridging.
2. Define advanced UI extensions such as preset profiles and guided
   calibration.
3. Define remote logging channel requirements.
4. Define GPS-driven area and speed fallback requirements.
5. Create feature-specific acceptance criteria and regression gates.

### P9 Outputs

- Optional feature specs with acceptance criteria.
- Regression plan protecting the single-Nano baseline.

### P9 Validation

- Verify each optional enhancement has explicit acceptance criteria and
  regression scope.
- Verify no optional enhancement changes the tagged baseline without a new
  controlled phase entry.

### P9 Failure / Recovery

- If an optional enhancement breaks the tagged baseline, revert to the release
  tag and re-scope the work into a new controlled phase.
