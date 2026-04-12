# TESTING.md

Spray Controller Testing Procedures

## Bench Tests

- Verify pump PWM output on pin `11`.
- Verify total-flow pulse input on pin `3`.
- Verify wheel pulse input on pin `2`.
- Verify boom section outputs on pins `12`, `13`, and `A1`.
- Verify run/hold input on pin `A3` and section switch inputs on `A4`,
  `A5`, and `4`.

## Deterministic Compile Verification

- Install Arduino CLI and AVR core:
  - `chmod +x ./ci/setup-arduino-cli.sh`
  - `./ci/setup-arduino-cli.sh`
- Compile both supported board targets:
  - `FQBN=arduino:avr:nano ./ci/compile-sketch.sh`
  - `FQBN=arduino:avr:uno ./ci/compile-sketch.sh`

## Canonical Validation Gate

- Command: `./scripts/validate.sh`
- Expected output markers (all required):
  - `== YAML schema validation ==`
  - `== Protocol structure check ==`
  - `== Config completeness check ==`
  - `== PIN-WAVE-001 pin-map check ==`
  - `== PIN-WAVE-002 parity check ==`
  - `== PIN-WAVE-003 Mega policy check ==`
  - `== PIN-WAVE-004 hardware doc parity check ==`
  - `== P3 sensor robustness check ==`
  - `== P3 closure artifact check ==`
  - `== P0 closure artifact check ==`
  - `== P1 closure artifact check ==`
  - `== P2 closure artifact check ==`
  - `== P4 closure artifact check ==`
  - `== P4 operator interface check ==`
  - `== P5 scalability invariant check ==`
  - `== P5 closure artifact check ==`
  - `== P6 flow calibration workflow check ==`
  - `== P6 wheel calibration workflow check ==`
  - `== P6 calibration storage check ==`
  - `== P6 calibration override check ==`
  - `== P6 closure artifact check ==`
  - `== P7 documentation alignment check ==`
  - `== SUCCESS ==`

- Optional artifact capture:
  - `./scripts/run_validation_and_capture.sh`
  - Writes pass/fail transcript to `validation/validation_pass.txt`.

## PIN-WAVE-001 Pin-Map Validator

- Command: `python3 scripts/validate_pin_wave_001.py`
- Expected pass marker: `pin_wave_001_ok`
- Deterministic checks enforced:
  - Required board profiles exist in `pins.yaml` (`nano`, `uno`, `mega2560`).
  - Required pin keys exist per profile and are unique within each profile.
  - `wheel_sensor` and `flow_sensor` both satisfy board interrupt-pin policy.
  - `pump_pwm` is restricted to board PWM-capable pins.

## PIN-WAVE-002 Pin-Parity Validator

- Command: `python3 scripts/validate_pin_wave_002.py`
- Expected pass marker: `pin_wave_002_ok`
- Deterministic checks enforced:
  - `pins.yaml` Nano/Uno profiles match `include/pins_nano.h` and
    `include/pins_uno.h` constants for wheel/flow/pump, boom outputs, LED, and
    switch inputs.
  - `HARDWARE.md` preserves the shared Uno/Nano mapping policy statement.
  - `include/pins.h` preserves the Uno/Nano-only build target selector policy.

## PIN-WAVE-003 Mega-Profile Policy Validator

- Command: `python3 scripts/validate_pin_wave_003.py`
- Expected pass marker: `pin_wave_003_ok`
- Deterministic checks enforced:
  - `pins.yaml` Mega2560 profile preserves required keys for sensors, pump,
    sections, run/hold, and optional pressure input.
  - Mega wheel/flow inputs remain interrupt-capable and do not collide; pump PWM
    remains on a PWM-capable Mega pin.
  - Mega section/run/pressure fields remain constrained to the documented
    D22-D53 / A8-A15 assignment range.
  - `include/pins.h` and `HARDWARE.md` preserve aligned board target policy
    declarations.

## PIN-WAVE-004 Hardware-Doc Parity Validator

- Command: `python3 scripts/validate_pin_wave_004.py`
- Expected pass marker: `pin_wave_004_ok`
- Deterministic checks enforced:
  - Mega functional pin constants in `include/pins_mega2560.h` match the
    Mega pinout table in `HARDWARE.md`.
  - Mega LCD SPI/reset constants in `include/pin_map_lcd.h` match the Mega
    LCD rows in `HARDWARE.md`.
  - `PIN_BTN_SELECT` must not collide with `LCD_RESET`.

## P3 Deterministic Robustness Validator

- Command: `python3 scripts/validate_p3_sensor_robustness.py`
- Expected pass marker: `p3_sensor_robustness_ok`
- Deterministic checks enforced:
  - Wheel timeout constant + timeout-to-zero behavior in `readSpeed()`.
  - Flow stale-timeout constant + stale fallback and max clamp behavior.
  - Pressure telemetry compile-time guard
    (`ENABLE_PRESSURE_SENSOR=false` by default).
  - Status telemetry schema includes `fault_bits` and `fault_text`.

## P3 Deterministic Closure Validator

- Command: `python3 scripts/validate_p3_closure.py`
- Expected pass marker: `p3_closure_ok`
- Deterministic checks enforced:
  - `PHASE_THREE_READINESS_AUDIT.md` preserves the canonical
    **Ready to close P3 (GO).** closure decision.
  - The Phase-Three audit continues to cite both
    `./scripts/validate.sh` and
    `python3 scripts/validate_p3_sensor_robustness.py` as deterministic
    evidence.
  - Phase-Three closure continues to require deterministic
    fault-telemetry visibility (`fault_bits`, `fault_text`) and a
    `CHANGELOG.md` closure record.

## P1 Deterministic Closure Validator

- Command: `python3 scripts/validate_p1_closure.py`
- Expected pass marker: `p1_closure_ok`
- Deterministic checks enforced:
  - `PHASE_ONE_READINESS.md` records a canonical **GO for P1 closure** decision.
  - The Phase-One audit cites the standard `./scripts/validate.sh`
    gate with the required closure markers.
  - Phase-One closure alignment is explicitly stated across
    `pins.yaml`, `config.yaml`, `CONFIGURATION.md`, and
    `HARDWARE.md`.
  - `CHANGELOG.md` records the Phase-One closure task and the
    canonical gate includes the P1 closure check.

## P2 Deterministic Closure Validator

- Command: `python3 scripts/validate_p2_closure.py`
- Expected pass marker: `p2_closure_ok`
- Deterministic checks enforced:
  - `PHASE_TWO_READINESS_AUDIT.md` records a canonical
    **GO: P2 is ready to close.** decision.
  - The Phase-Two audit cites the standard
    `./scripts/validate.sh` gate with the required closure markers,
    including `p2_closure_ok`.
  - `Spray-controller.ino` preserves the fixed SEE -> THINK -> DO loop order:
    section sample, sensor reads, active-width computation, run/hold-safe pump
    command, and compact `ST:` status publish.
  - `CHANGELOG.md` records the Phase-Two closure task and the canonical gate
    includes the P2 closure check.

## P4 Deterministic Operator Interface Validator

- Command: `python3 scripts/validate_p4_operator_interface.py`
- Expected pass marker: `p4_operator_interface_ok`
- Deterministic checks enforced:
  - Menu state/event contract and explicit transitions
    (`HOME`, `MENU`, `COUNTERS`, `RESET_CONFIRM`).
  - `ME:NAV|SEL|CAN|CFM` event parsing and `MS:` state output wiring.
  - Fixed-cadence preview gate (`PREVIEW_INTERVAL_MS`) and
    bounded `PV` publish path in loop cadence.
  - Distance/area counter update path uses wheel speed and active
    section width every deterministic loop tick.
  - Reset action requires confirm token path and emits
    `RS:COUNTERS_CALIBRATION_RESET` only on confirmed reset.

## P4 Deterministic Closure Validator

- Command: `python3 scripts/validate_p4_closure.py`
- Expected pass marker: `p4_closure_ok`
- Deterministic checks enforced:
  - `PHASE_FOUR_READINESS_AUDIT.md` preserves the canonical
    **Ready to close P4 (GO).** closure decision.
  - The Phase-Four audit continues to cite both
    `./scripts/validate.sh` and
    `python3 scripts/validate_p4_operator_interface.py` as deterministic
    evidence.
  - Phase-Four closure continues to require deterministic
    operator reset/menu contract coverage and a `CHANGELOG.md` closure record.

## P5 Deterministic Telemetry Mapping Validator

- Command: `python3 scripts/validate_protocols.py`
- Expected pass marker: `protocols_ok`
- Deterministic checks enforced:
  - Stable `S:` section telemetry field contract and ascending section ordering.
  - Stable `SN:` sensor ID mapping and per-sensor field ordering.
  - Backward-compatible `ST:` status frame remains documented.

## P5 Deterministic Scalability Invariant Validator

- Command: `python3 scripts/validate_p5_scalability.py`
- Expected pass marker: `p5_scalability_ok`
- Deterministic checks enforced:
  - `SECTION_COUNT` matches descriptor-table length on Nano and Uno pin maps.
  - Section descriptor IDs remain ascending and stable for
    backward-compatible 3-section output.
  - Sensor telemetry IDs remain fixed (`flow=0`, `wheel=1`,
    optional `pressure=2`).
  - Firmware status bitmask generation and telemetry loops stay
    descriptor/contract driven.

## P5 Deterministic Closure Validator

- Command: `python3 scripts/validate_p5_closure.py`
- Expected pass marker: `p5_closure_ok`
- Deterministic checks enforced:
  - `PHASE_FIVE_READINESS_AUDIT.md` preserves the canonical
    **Ready to close P5 (GO).** closure decision.
  - The Phase-Five audit continues to cite both
    `./scripts/validate.sh` and
    `python3 scripts/validate_p5_scalability.py` as deterministic
    evidence.
  - Phase-Five closure continues to require deterministic
    descriptor-driven topology + P5 invariants coverage and a
    `CHANGELOG.md` closure record.

## P6 Flow Calibration Workflow Validator

- Command: `python3 scripts/validate_p6_flow_calibration.py`
- Expected pass marker: `p6_flow_calibration_ok`
- Deterministic checks enforced:
  - `CALIBRATION.md` includes an executable 5-step flow calibration sequence.
  - Documented sanity window for accepted `FLOW_PULSES_PER_LITER` values.
  - Accepted flow calibration is stored persistently with
    validity fallback to the `config.h` default at boot.

## P6 Wheel Calibration Workflow Validator

- Command: `python3 scripts/validate_p6_wheel_calibration.py`
- Expected pass marker: `p6_wheel_calibration_ok`
- Deterministic checks enforced:
  - `CALIBRATION.md` includes an executable 5-step wheel calibration sequence.
  - Documented acceptance window for `wheel_distance_per_pulse_m`.
  - Accepted wheel calibration is stored persistently with
    validity fallback to the `config.h` defaults at boot.

## P6 Calibration Storage Validator

- Command: `python3 scripts/validate_p6_calibration_storage.py`
- Expected pass marker: `p6_calibration_storage_ok`
- Deterministic checks enforced:
  - Dedicated calibration persistence module exists with a
    stored validity marker and checksum.
  - Boot path reloads persisted calibration and falls back to
    `config.h` defaults when storage is blank or invalid.
  - Flow and wheel sensor conversions consume the active
    persisted calibration profile instead of compile-time constants
    directly.

## P6 Calibration Override Validator

- Command: `python3 scripts/validate_p6_calibration_override.py`
- Expected pass marker: `p6_calibration_override_ok`
- Deterministic checks enforced:
  - Calibration persistence block stores an operator override selector.
  - Runtime calibration accessors resolve to defaults when override is enabled.
  - `CALIBRATION.md` documents the defaults-vs-calibrated operator workflow.

## P6 Deterministic Closure Validator

- Command: `python3 scripts/validate_p6_closure.py`
- Expected pass marker: `p6_closure_ok`
- Deterministic checks enforced:
  - Operator-interface command contract includes explicit flow and wheel
    calibration entrypoint events.
  - Entry events are accepted from operator commands and surfaced as
    deterministic telemetry markers.
  - `CHANGELOG.md` records `P6-CLS-001` closure and
    `ROADMAP_CLOSURE_TASKS.md` marks the closure task complete.

## P7 Documentation Alignment Validator

- Command: `python3 scripts/validate_p7_documentation.py`
- Expected pass marker: `p7_documentation_ok`
- Deterministic checks enforced:
  - Core docs align with current protocol tokens, cadence, and reset behavior.
  - Core docs record the current three-section pin map and key
    control constants.
  - No `## Placeholders` section remains in `PROTOCOLS.md`,
    `USAGE.md`, or `TESTING.md`.

## Synthetic Event Tests

- Simulate run/hold switch toggle and verify `run=0` drives pump duty to `0`.
- Simulate total-flow sensor dropouts/spikes and verify stale
  fault bit behavior.
- Toggle all section combinations and verify active width
  calculation in `0.5 m` increments.
- Validate reset-confirm workflow:
  - Send `ME:NAV`, `ME:SEL`, `ME:SEL`, `ME:CFM` over serial.
  - Verify `MS:` state transitions and a single
    `RS:COUNTERS_CALIBRATION_RESET` frame on confirm.
  - Repeat with `ME:CAN` in `RESET_CONFIRM` and verify no `RS:` frame.

## Calibration Procedures

- Map PWM duty cycle to total flow rate across the intended operating range.
- Verify total-flow sensor readings against a known-volume timed
  collection test.
- Validate rate-control response using speed (km/h), width (m),
  and target rate (`100.0 L/ha`).
- Verify wheel timeout and flow stale timeout behavior at
  `1200 ms` fault thresholds.
