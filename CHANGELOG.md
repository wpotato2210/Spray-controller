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

- **0.7** (2026-03-22): P1 closed. Finalized `pins.yaml` + `config.yaml`, aligned `CONFIGURATION.md`/`HARDWARE.md`, validated with `./scripts/validate.sh` (success markers: `yamllint_ok`, `protocols_ok`, `config_ok`), and recorded pass artifact `validation/validation_pass.txt`.

- **0.8** (2026-03-22): Executed `P1-GATE-005` readiness re-audit; confirmed `P1-GATE-001` through `P1-GATE-004` are complete and recorded **GO** decision to proceed to P2 in `PHASE_TWO_READINESS_AUDIT.md`.

- **0.9** (2026-03-22): Completed P3 readiness audit in `PHASE_THREE_READINESS_AUDIT.md`; decision: **NO-GO for P3 closure** pending blocker tasks `P3-TSK-001` through `P3-TSK-003`.

## Placeholders

- Update dates.
- Notes for future releases and bug fixes.
