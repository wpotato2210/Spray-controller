# Phase-Three Readiness Audit (P3 Gate Check)

Date: 2026-03-22 (UTC)

Scope: Assess implementation readiness against P3 requirements in
`DEVELOPMENT_ROADMAP.md` and produce a prioritized closure task list.

## Verdict

**Not ready to close P3.**

Reason: one P3 item is partially implemented and three P3 items are missing.

## Evidence reviewed

- P3 deterministic tasks and deliverables from `DEVELOPMENT_ROADMAP.md`.
- Sensor implementations:
  - `src/wheel_sensor.cpp`
  - `src/flow_sensor.cpp`
- Interface surface in `include/interfaces.h`.
- Runtime control/fault fallback behavior in `src/flow_controller.cpp`.
- Telemetry frame contract in `PROTOCOLS.md` and runtime status output in
  `Spray-controller.ino`.
- Deterministic validation command executed on 2026-03-22 (UTC):
  - `./scripts/validate.sh`
  - Result: success (`yamllint_ok`, `protocols_ok`, `config_ok`,
    `== SUCCESS ==`).

## P3 deterministic task status

| P3 task (roadmap) | Status | Findings |
| --- | --- | --- |
| Wheel pulse timeout + forced zero-speed handling | **Partial** |
|  |  | No explicit timeout. |
| Flow plausibility clamp + stale-read fallback | **Partial** |
|  |  | Clamp exists; stale fallback missing. |
| Pressure abstraction (`readPressure()`), telemetry-only | **Missing** |
|  |  | No module or compile-time guard path. |
| Sensor fault flags in protocol/debug output | **Missing** |
|  |  | No exported fault bitfield today. |

## Deliverable status snapshot

| P3 deliverable (roadmap) | Status | Evidence summary |
| --- | --- | --- |
| `flow_sensor` + `wheel_sensor` bounded in no-pulse and burst cases |
| **Partial** | Timeout/fallback gaps remain. |
| Optional pressure module guarded and disabled by default | **Missing** |
|  |  | No pressure abstraction path. |
| Fault bitfield and status text in telemetry | **Missing** |
|  |  | `ST` has no fault field. |

## Prioritized closure task list

| Priority | Task ID | Task | Rationale | Exit criteria |
| --- | --- | --- | --- | --- |
| P0 (blocker) | P3-TSK-001 | Add wheel pulse timeout in `readSpeed()`. |
|  |  |  | Stops stale speed when pulses stop. |
|  |  |  |  | Timeout forces speed `0.0`; test proves it. |
| P0 (blocker) | P3-TSK-002 | Add flow stale-read timeout + bounded fallback. |
|  |  |  | Completes fallback requirement. |
|  |  |  |  | Timeout path documented and deterministic. |
| P0 (blocker) | P3-TSK-003 | Add protocol fault bitfield in status telemetry. |
|  |  |  | Makes faults externally visible. |
|  |  |  |  | `ST` includes fault field and docs match. |
| P1 | P3-TSK-004 | Add optional pressure abstraction (default off). |
|  |  |  | Enables diagnostics without control coupling. |
|  |  |  |  | Feature-gated pressure telemetry compiles. |
| P1 | P3-TSK-005 | Add deterministic tests and document in `TESTING.md`. |
|  |  |  | Needed for repeatable closure evidence. |
|  |  |  |  | Test steps + pass artifacts committed. |

## Recommended go/no-go decision

- **NO-GO for P3 closure** until all P0 tasks (`P3-TSK-001` through
  `P3-TSK-003`) are complete.
- Begin with telemetry fault visibility (`P3-TSK-003`) in parallel with sensor
  timeout/fallback implementation to reduce integration rework.
