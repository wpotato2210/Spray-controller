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
  `config_ok`), and recorded pass artifact
  `validation/validation_pass.txt`.

- **0.8** (2026-03-22): Executed `P1-GATE-005` readiness re-audit; confirmed
  `P1-GATE-001` through `P1-GATE-004` are complete and recorded **GO**
  decision to proceed to P2 in `PHASE_TWO_READINESS_AUDIT.md`.

- **0.9** (2026-03-22): Completed P3 readiness audit in
  `PHASE_THREE_READINESS_AUDIT.md`; decision: **NO-GO for P3 closure**
  pending blocker tasks `P3-TSK-001` through `P3-TSK-003`.

- **0.10** (2026-03-22): Executed `P3-TSK-002` by adding explicit flow stale-timeout fallback in `FlowSensor::readFlow()` using `FLOW_STALE_TIMEOUT_MS` + bounded fallback output `FLOW_STALE_FALLBACK_LPM` (default `0.0`), while preserving burst clamp behavior via `MAX_FLOW_LPM`.
- **0.11** (2026-03-22): Executed `P3-TSK-003` by extending `ST` telemetry with deterministic fault visibility fields (`fault_bits`, `fault_text`) and wiring fault source bits for flow stale timeout, wheel timeout, and sensor-config validity.
- **0.12** (2026-03-22): Executed `P3-TSK-004` by adding compile-time gated pressure telemetry abstraction (`PressureSensor::readPressure()`), default-disabled gate (`ENABLE_PRESSURE_SENSOR=false`), optional `PR:<pressure_kpa>` output, and pressure config fault bit wiring when enabled.

- **0.13** (2026-03-22): Re-ran Phase-Three readiness audit with deterministic checks (`./scripts/validate.sh`, `python3 scripts/validate_p3_sensor_robustness.py`), confirmed all P3 deliverables complete, and recorded **GO** decision in `PHASE_THREE_READINESS_AUDIT.md`.

- **0.14** (2026-03-22): Executed Phase-Four readiness audit in `PHASE_FOUR_READINESS_AUDIT.md`; decision: **NO-GO for P4 closure** because operator menu state machine, distance/area accumulators, and explicit reset-confirm workflows are not yet implemented.


- **0.15** (2026-03-22): Refined `PHASE_FOUR_READINESS_AUDIT.md` with a deterministic remediation task list (`P4-TSK-001` through `P4-TSK-006`) so each P4 closure action is uniquely tracked.

## Placeholders

- Update dates.
- Notes for future releases and bug fixes.
