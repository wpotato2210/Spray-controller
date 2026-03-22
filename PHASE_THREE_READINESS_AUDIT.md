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
  - Result: success (`yamllint_ok`, `protocols_ok`, `config_ok`, `== SUCCESS ==`).

## P3 deterministic task status

| P3 task (roadmap) | Status | Findings |
| --- | --- | --- |
| Add pulse timeout and zero-speed handling for wheel sensor | **Partial** | Zero-speed naturally occurs at zero pulse delta, but no explicit pulse-timeout guard exists to force bounded behavior after pulse-loss windows. |
| Add flow plausibility clamp and stale-read fallback | **Partial** | Upper clamp exists (`MAX_FLOW_LPM`) and non-negative sanitization exists; stale-read fallback behavior is not explicitly implemented/documented. |
| Add optional pressure sensor abstraction (`readPressure()`), telemetry only, disabled by default | **Missing** | No pressure interface/module and no compile-time pressure flag path found. |
| Define sensor fault flags for protocol and debug output | **Missing** | Control fallback exists internally (`no_flow_ms_` in flow controller), but no exported fault bitfield/flags in protocol output or docs. |

## Deliverable status snapshot

| P3 deliverable (roadmap) | Status | Evidence summary |
| --- | --- | --- |
| `flow_sensor` and `wheel_sensor` produce bounded values under no-pulse and burst conditions | **Partial** | Burst clamp exists for flow; explicit wheel pulse-timeout and stale-flow fallback still absent. |
| Optional pressure module compile-time guarded and disabled by default | **Missing** | No pressure abstraction and no compile-time guard path present. |
| Fault bitfield and status text added to telemetry definition | **Missing** | Current `ST` frame has `flow,pwm,run,section_mask` only; no fault field. |

## Prioritized closure task list

| Priority | Task ID | Task | Rationale | Exit criteria |
| --- | --- | --- | --- | --- |
| P0 (blocker) | P3-TSK-001 | Implement wheel pulse-timeout guard in `WheelSensor::readSpeed()` and force deterministic zero-speed on timeout windows. | Directly required by P3 task #1; prevents stale-speed influence when pulses stop unexpectedly. | Configurable timeout constant enforced; test path confirms speed goes to `0.0` after timeout with no pulses. |
| P0 (blocker) | P3-TSK-002 | Implement flow stale-read fallback policy in `FlowSensor::readFlow()` (with explicit timeout + bounded fallback output). | Completes P3 task #2 beyond clamp-only behavior; prevents ambiguous stale measurements. | Timeout path implemented and documented; no-pulse and burst scenarios remain bounded and deterministic. |
| P0 (blocker) | P3-TSK-003 | Add protocol-visible sensor fault flags (bitfield) and include them in status telemetry and protocol docs. | Required for P3 task #4 and P3 deliverable #3; current internal fault behavior is not externally visible. | `ST` frame includes fault field; `PROTOCOLS.md` documents bits and meanings; debug output references same flags. |
| P1 | P3-TSK-004 | Add optional pressure abstraction (`readPressure()`), compile-time guarded and disabled by default, telemetry-only. | Required by P3 task #3; keeps pressure decoupled from control while enabling diagnostics. | Interface + module compile with feature disabled by default; enabling flag adds pressure telemetry without altering pump control logic. |
| P1 | P3-TSK-005 | Add deterministic tests/bench checks for wheel timeout, flow stale fallback, and fault-flag transitions; document in `TESTING.md`. | Needed to close P3 with reproducible evidence, consistent with prior gate discipline. | Test procedure documented; commands and expected markers defined; pass artifacts recorded in repo. |

## Recommended go/no-go decision

- **NO-GO for P3 closure** until all P0 tasks (`P3-TSK-001` through
  `P3-TSK-003`) are complete.
- Begin with telemetry fault visibility (`P3-TSK-003`) in parallel with sensor
  timeout/fallback implementation to reduce integration rework.
