# Phase-Four Readiness Audit (P4 Gate Check)

Date: 2026-03-22 (UTC)

Scope: Assess implementation readiness against P4 requirements in
`DEVELOPMENT_ROADMAP.md` and issue a gate decision for P5 entry.

## Verdict

**Not ready to close P4 (NO-GO).**

Reason: the current firmware and docs do not yet implement an operator menu
state machine, live preview UI fields, distance/area accumulators, or explicit
reset confirmation flows required by P4.

## Evidence reviewed

- P4 deterministic tasks and deliverables from `DEVELOPMENT_ROADMAP.md`.
- Runtime loop and telemetry in `Spray-controller.ino`.
- Protocol contract in `PROTOCOLS.md`.
- Operator guidance in `USAGE.md`.
- Deterministic validation command executed on 2026-03-22 (UTC):
  - `./scripts/validate.sh`
  - Result: success (`yamllint_ok`, `protocols_ok`, `config_ok`,
    `p3_sensor_robustness_ok`, `== SUCCESS ==`).

## P4 deterministic task status

| P4 task (roadmap) | Status | Findings |
| --- | --- | --- |
| Implement menu navigation states | **Missing** | No operator menu state machine module, interface, or loop path present. |
| Add live preview fields (speed, flow, duty, active sections) | **Partial** | Runtime computes speed/flow/duty/sections and publishes telemetry, but no operator menu/UI preview layer exists. |
| Add distance and area accumulators from wheel data and active width | **Missing** | No distance or area counters, accumulation logic, or storage path found. |
| Add reset confirmation workflow for counters and calibration | **Missing** | No confirmation state workflow or reset command path for counters/calibration. |

## Deliverable status snapshot

| P4 deliverable (roadmap) | Status | Evidence summary |
| --- | --- | --- |
| Operator menu state machine documented and tested | **Missing** | `USAGE.md` still lists operator interface as placeholder; no menu protocol/docs/tests found. |
| Live preview updates at fixed interval | **Partial** | Fixed-interval telemetry exists (`TELEMETRY_INTERVAL_MS`), but it is serial status output rather than operator menu preview. |
| Distance and area reset requires explicit confirm action | **Missing** | No distance/area accumulators are implemented; therefore no reset confirmation path exists. |

## Gate decision

- **NO-GO: Keep P4 open.**
- Recommended next deterministic closure path:
  1. Add operator-interface module(s) with explicit menu states.
  2. Add distance/area accumulation and reset-confirm transitions.
  3. Add deterministic checks for P4 behaviors and wire them into
     `./scripts/validate.sh`.
  4. Update `USAGE.md`/`PROTOCOLS.md` with finalized P4 behavior, then re-audit.
