# Phase-Four Readiness Audit (P4 Gate Check)

Date: 2026-03-22 (UTC)

Scope: Assess implementation readiness against P4 requirements in
`DEVELOPMENT_ROADMAP.md` and issue a gate decision for P4 closure readiness.

## Verdict

**Not ready to close P4 (NO-GO).**

Reason: P4 operator-interface deterministic tasks are not yet implemented in
firmware, protocol surface, or documentation. Current implementation remains
focused on core control-loop and compact telemetry, with no explicit operator
menu workflow.

## Evidence reviewed

- P4 deterministic tasks/deliverables in `DEVELOPMENT_ROADMAP.md`.
- Runtime firmware loop and telemetry output in `Spray-controller.ino`.
- Frozen interfaces in `include/interfaces.h`.
- Protocol definitions in `PROTOCOLS.md` and `include/protocol.h`.
- Operator/user guidance in `USAGE.md`, `ARCHITECTURE.md`, and `DIAGRAMS.md`.

## P4 deterministic task status

|P4 task (roadmap)|Status|Findings|
|---|---|---|
|Implement menu navigation states|**Missing**|No menu/state-machine module or interfaces exist; loop performs only sensor read/control/telemetry cycle.|
|Add live preview fields (speed, flow, duty, active sections)|**Partial**|Loop telemetry publishes flow/duty/run/sections/faults, but does not publish speed in `ST` frame and no operator UI preview surface exists.|
|Add distance and area accumulators based on wheel data and active width|**Missing**|No accumulator variables, services, protocol fields, or docs for distance/area counters.|
|Add reset confirmation workflow for counters and calibration|**Missing**|No reset workflow/state confirmation path in firmware or protocol; docs contain placeholder operator-interface guidance.|

## Deliverable status snapshot

|P4 deliverable (roadmap)|Status|Evidence summary|
|---|---|---|
|Operator menu state machine documented and tested|**Missing**|No menu state machine in code/docs; no P4-specific tests in `TESTING.md` or scripts.|
|Live preview updates at fixed interval|**Missing**|Telemetry interval exists, but no dedicated operator preview contract including all required fields.|
|Distance and area reset requires explicit confirm action|**Missing**|No counters and no confirm/reset protocol/UI behavior implemented.|

## Prioritized task list

|Priority|Task ID|Task|Why this priority|Exit criteria|
|---|---|---|---|---|
|P0|`P4-TSK-001`|Define operator-interface contract (menu states, preview fields, reset-confirm sequence) in `PROTOCOLS.md` + `USAGE.md`.|Unblocks deterministic implementation and testing; prevents interface churn.|Docs define state names/transitions, required preview payload (`speed,flow,duty,active_sections,distance,area`), and confirm-reset handshake.|
|P0|`P4-TSK-002`|Implement firmware menu state machine module and integrate in main loop with deterministic update cadence.|Core P4 objective; all other UI features depend on stable state handling.|New state machine code compiled for Nano/Uno, with explicit transitions for navigate/select/confirm/cancel.|
|P1|`P4-TSK-003`|Extend runtime preview pipeline to include speed + existing fields at fixed interval.|Provides immediate operator visibility and validates state-machine output path.|Preview frame includes speed, flow, duty, active sections with documented fixed publish interval.|
|P1|`P4-TSK-004`|Add deterministic distance/area accumulators using wheel speed and active spray width.|Needed for field metrics and reset workflow; relies on stable loop timing.|Counters update each loop with bounded numeric behavior and are exposed in preview/status contract.|
|P1|`P4-TSK-005`|Implement explicit reset confirmation workflow for counters and calibration data.|Safety requirement in roadmap; prevents accidental destructive resets.|Two-step confirm action required; reset events observable in telemetry/log output.|
|P2|`P4-TSK-006`|Add validation tests/scripts for P4 scenarios (menu navigation, preview cadence, counter reset confirmation).|Required to close gate reproducibly and guard regressions.|Deterministic checks added to validation scripts and documented in `TESTING.md` with pass markers.|
|P2|`P4-TSK-007`|Close documentation placeholders in `DIAGRAMS.md` and operator sections after implementation.|Needed for phase closure quality and future reproducibility.|No operator-interface placeholders remain; state and I/O diagrams match implemented behavior.|

## Gate decision

- **NO-GO for P4 closure readiness.**
- Start with `P4-TSK-001` and `P4-TSK-002` before implementing counters/reset
  logic.
- Re-audit P4 after `P4-TSK-001` through `P4-TSK-006` are complete.
