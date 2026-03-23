# Spray Controller Change Log

## Versions

- **0.1** (TBD): Initial prototype firmware.
- **0.2** (TBD): Added frozen module interfaces and flow sensor support.
- **0.3** (TBD): Added run/hold switch and bypass solenoid logic.
- **0.4** (2026-03-22): Added deterministic phased development roadmap

(P0-P9).

- **0.5** (2026-03-22): Implemented REC-004 telemetry throttling with

TX-capacity gating to reduce control-loop jitter risk.

- **0.6** (2026-03-22): Implemented REC-005 compact `ST` telemetry frame

(`flow,pwm,run,section_mask`) to reduce per-cycle serial overhead.

- **0.7** (2026-03-22): P1 closed. Finalized `pins.yaml` + `config.yaml`,
  aligned `CONFIGURATION.md`/`HARDWARE.md`, validated with
  `./scripts/validate.sh` (success markers: `yamllint_ok`, `protocols_ok`,
  `config_ok`), and recorded pass artifact `validation/validation_pass.txt`.

- **0.8** (2026-03-22): Executed `P1-GATE-005` readiness re-audit; confirmed
  `P1-GATE-001` through `P1-GATE-004` are complete and recorded **GO** decision
  to proceed to P2 in `PHASE_TWO_READINESS_AUDIT.md`.

- **0.9** (2026-03-22): Completed P3 readiness audit in
  `PHASE_THREE_READINESS_AUDIT.md`; decision: **NO-GO for P3 closure** pending
  blocker tasks `P3-TSK-001` through `P3-TSK-003`.

- **0.10** (2026-03-22): Executed `P3-TSK-002` by adding explicit flow
  stale-timeout fallback in `FlowSensor::readFlow()` using
  `FLOW_STALE_TIMEOUT_MS` + bounded fallback output `FLOW_STALE_FALLBACK_LPM`
  (default `0.0`), while preserving burst clamp behavior via `MAX_FLOW_LPM`.
- **0.11** (2026-03-22): Executed `P3-TSK-003` by extending `ST` telemetry with
  deterministic fault visibility fields (`fault_bits`, `fault_text`) and wiring
  fault source bits for flow stale timeout, wheel timeout, and sensor-config
  validity.
- **0.12** (2026-03-22): Executed `P3-TSK-004` by adding compile-time gated
  pressure telemetry abstraction (`PressureSensor::readPressure()`),
  default-disabled gate (`ENABLE_PRESSURE_SENSOR=false`), optional
  `PR:<pressure_kpa>` output, and pressure config fault bit wiring when enabled.

- **0.13** (2026-03-22): Re-ran Phase-Three readiness audit with deterministic
  checks (`./scripts/validate.sh`, `python3
  scripts/validate_p3_sensor_robustness.py`), confirmed all P3 deliverables
  complete, and recorded **GO** decision in `PHASE_THREE_READINESS_AUDIT.md`.

- **0.14** (2026-03-22): Executed Phase-Zero readiness audit in
  `PHASE_ZERO_READINESS_AUDIT.md`; decision: **NO-GO for P0 closure** pending
  missing deliverables for DEV_NOTES loop ownership mapping and a frozen
  interface checklist mapped to source implementations.

- **0.15** (2026-03-22): Executed Phase-Four readiness audit in
  `PHASE_FOUR_READINESS_AUDIT.md`; decision: **NO-GO for P4 closure readiness**
  with prioritized task backlog `P4-TSK-001` through `P4-TSK-007`.

- **0.16** (2026-03-22): Executed `P4-TSK-001` by defining the
  deterministic operator-interface contract in `PROTOCOLS.md` and `USAGE.md`,
  including menu states/transitions, required `PV` preview payload fields
  (`speed_kmh,flow_lpm,pump_duty,active_sections,distance_m,area_ha`), fixed
  preview cadence rules, and explicit reset-confirm handshake semantics.

- **0.17** (2026-03-22): Executed `P4-TSK-003` by implementing fixed-cadence
  `PV` preview publishing in the main loop with
  `speed_kmh,flow_lpm,pump_duty,active_sections,distance_m,area_ha` payload
  order and deterministic protocol prefix wiring (`MSG_PREVIEW_PREFIX`).

- **0.18** (2026-03-22): Executed `P4-TSK-005` by implementing explicit
  two-step reset confirmation execution in firmware (`ME:*` input events to
  `RESET_CONFIRM` + `CFM`), deterministic reset action for counters and runtime
  calibration/sensor reset hooks, and observable reset/state telemetry frames
  (`MS:` and `RS:COUNTERS_CALIBRATION_RESET`).

- **0.19** (2026-03-22): Executed `P4-TSK-006` by adding deterministic
  P4 validation script `scripts/validate_p4_operator_interface.py`,
  integrating it into canonical `./scripts/validate.sh` gate checks,
  and documenting validator command/pass marker plus enforced
  scenario coverage in `TESTING.md`.

- **0.20** (2026-03-22): Executed `P4-TSK-007` by replacing
  `DIAGRAMS.md` placeholders with implemented deterministic diagrams for
  SEE/THINK/DO architecture, operator menu transitions, preview/reset I/O
  dependencies, and logical wiring aligned to `ME`/`ST`/`PV`/`MS`/`RS`
  protocol behavior.

- **0.21** (2026-03-22): Re-ran Phase-Four readiness audit in
  `PHASE_FOUR_READINESS_AUDIT.md`; confirmed all P4 deterministic tasks and
  deliverables are complete, and recorded **GO** decision to proceed to P5.

- **0.22** (2026-03-22): Executed Phase-Five readiness audit in
  `PHASE_FIVE_READINESS_AUDIT.md`; decision: **NO-GO for P5 closure readiness**
  with prioritized task backlog `P5-TSK-001` through `P5-TSK-006`.

- **0.24** (2026-03-23): Executed `P5-TSK-003` by formalizing
  deterministic section/sensor telemetry contracts in `include/protocol.h`,
  routing firmware emission through ordered telemetry contract tables, and
  documenting the stable ID mapping rules in `PROTOCOLS.md`.
- **0.25** (2026-03-23): Executed `P5-TSK-004` by tightening the
  hardware adapter boundary: section switch/output access is now cached
  behind `ArduinoSectionHardwareAdapter`, and Phase Five audit evidence
  now records the control path as adapter-driven instead of direct GPIO.

- **0.26** (2026-03-23): Executed `P5-TSK-005` by finalizing
  `ARCHITECTURE.md` with scalable control-core, frozen-adapter, and
  concrete-transport boundaries, explicit dependency direction, and
  multi-MCU bridge-ready seams for future distributed I/O.

## Placeholders

- Update dates.
- Notes for future releases and bug fixes.
