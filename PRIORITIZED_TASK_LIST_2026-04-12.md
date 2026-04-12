# Prioritized Task List from Engineering Audit (2026-04-12)

## Wave Plan

| Wave ID | Priority | Objective | Exit Criteria |
| --- | --- | --- | --- |
| WAVE-01-MEM-PIN | P0 | Remove immediate hardware/firmware failure risks. | AVR display mode changed to page buffer, pin-policy asserts cover all assigned roles, docs reflect real dependencies. |
| WAVE-02-TIMING-EVENTS | P1 | Improve deterministic runtime behavior under load. | Atomic pulse snapshots, bounded multi-event dequeue, phase overrun metrics active. |
| WAVE-03-HAL-FAULTS | P2 | Improve portability and fault behavior consistency. | Display HAL backend selection + fault manager integrated behind stable interfaces. |
| WAVE-04-VALIDATION | P3 | Lock improvements with automated validation and HIL checks. | CI gates for board matrix, docs consistency, SRAM soak and serial stress tests defined and running. |

## Prioritized Tasks

| Task ID | Wave ID | Priority | Task | Deliverable | Depends On |
| --- | --- | --- | --- | --- | --- |
| TASK-001 | WAVE-01-MEM-PIN | P0 | Move AVR display from `U8G2_*_F_*` to page-buffer mode (`_1_` or `_2_`). | `src/display.cpp` updated + memory note in docs + smoke test evidence. | None |
| TASK-002 | WAVE-01-MEM-PIN | P0 | Expand pin-policy checks to validate all assigned role pins against LCD/SPI reserved sets. | `include/pin_policy_guards.h` static asserts for full assigned-pin matrix by board. | None |
| TASK-003 | WAVE-01-MEM-PIN | P0 | Align dependency docs with implementation (U8g2 required). | `README.md` / `INSTALL.md` updated with required library/version guidance. | None |
| TASK-004 | WAVE-02-TIMING-EVENTS | P1 | Add atomic pulse snapshot API to pulse counter adapter. | `PulseCounterAdapter` + Arduino implementation support single-lock snapshot reads. | TASK-001 |
| TASK-005 | WAVE-02-TIMING-EVENTS | P1 | Increase operator event dequeue budget per loop (bounded, e.g., 2-4). | Queue processing loop updated + overflow/depth telemetry retained. | None |
| TASK-006 | WAVE-02-TIMING-EVENTS | P1 | Add phase budget and overrun counters for loop phases. | Scheduler stats struct + telemetry/report output for overruns. | TASK-004, TASK-005 |
| TASK-007 | WAVE-03-HAL-FAULTS | P2 | Introduce display backend abstraction with compile-time backend selection. | `DisplayAdapter` interface + `ST7920_SPI` backend + `NONE` backend. | TASK-001 |
| TASK-008 | WAVE-03-HAL-FAULTS | P2 | Add centralized fault manager with latching/clear policy. | New module used by status fault publishing and runtime decisions. | TASK-006 |
| TASK-009 | WAVE-03-HAL-FAULTS | P2 | Replace repeated section descriptor scans with direct indexed access guards. | Section lookup simplification + static assertions for contiguous IDs. | None |
| TASK-010 | WAVE-04-VALIDATION | P3 | Add CI docs-consistency check between pin headers and hardware docs. | Script + CI job failing on pin-map drift. | TASK-002, TASK-003 |
| TASK-011 | WAVE-04-VALIDATION | P3 | Add UNO/Nano SRAM soak HIL test protocol for display+telemetry workload. | Repeatable HIL procedure with pass/fail limits. | TASK-001, TASK-006 |
| TASK-012 | WAVE-04-VALIDATION | P3 | Add serial burst/menu responsiveness HIL stress test. | Test script/protocol validating queue depth, overflow, and latency targets. | TASK-005, TASK-006 |

## Execution Order

1. WAVE-01-MEM-PIN
2. WAVE-02-TIMING-EVENTS
3. WAVE-03-HAL-FAULTS
4. WAVE-04-VALIDATION

## Recommended Next 3 Starts

1. Start `TASK-001` (highest user-visible stability risk on AVR).
2. Start `TASK-002` (prevents latent hardware regressions).
3. Start `TASK-003` (prevents failed bring-up due to missing dependency).
