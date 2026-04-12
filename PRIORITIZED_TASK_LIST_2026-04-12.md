# Prioritized Task List from Engineering Audit (2026-04-12)

## Wave Plan

| Wave ID | Priority | Objective | Exit Criteria |
| --- | --- | --- | --- |
| WAVE-01-HW-SAFETY | P0 | Remove hardware correctness risks that can cause system failure. | No pin conflicts, pin-policy asserts passing, hardware docs aligned with source headers. |
| WAVE-02-FW-STABILITY | P1 | Improve runtime determinism and fault resilience in control loop/telemetry/input handling. | Stable debounced inputs, bounded telemetry output, deterministic command/event handling. |
| WAVE-03-ARCH-PORTABILITY | P2 | Reduce coupling to Arduino runtime and improve testability/maintainability. | Time abstraction in place, loop services split, shared utilities consolidated. |
| WAVE-04-QUALITY-VALIDATION | P3 | Strengthen CI, validation coverage, and long-term hygiene. | Compile matrix + static checks + unit/HIL scaffolding operational. |

## Prioritized Tasks

| Task ID | Wave ID | Priority | Task | Deliverable | Depends On |
| --- | --- | --- | --- | --- | --- |
| TASK-HW-001 | WAVE-01-HW-SAFETY | P0 | Resolve Mega D49 conflict by remapping `PIN_BTN_SELECT` away from `LCD_RESET`. | Updated `include/pins_mega2560.h` + synchronized hardware docs. | None |
| TASK-HW-002 | WAVE-01-HW-SAFETY | P0 | Add compile-time uniqueness assertions for enabled pin roles per board. | `static_assert` guards for duplicate pin assignments. | TASK-HW-001 |
| TASK-HW-003 | WAVE-01-HW-SAFETY | P0 | Add feature-aware reserved-pin policy (SPI/LCD/UI domains). | Board pin policy checks for UNO/Nano/Mega. | TASK-HW-002 |
| TASK-HW-004 | WAVE-01-HW-SAFETY | P0 | Eliminate doc/code pin-map drift via single-source generation or validation. | Hardware pin table validation step in CI. | TASK-HW-001 |
| TASK-FW-001 | WAVE-02-FW-STABILITY | P1 | Implement debounce service for run/hold + section switches (20–40 ms configurable). | Stable input module + integration in loop path. | TASK-HW-003 |
| TASK-FW-002 | WAVE-02-FW-STABILITY | P1 | Add telemetry frame budgeting/backpressure (bounded frames per cycle). | Non-blocking telemetry scheduler with per-loop budget. | None |
| TASK-FW-003 | WAVE-02-FW-STABILITY | P1 | Introduce bounded operator event queue with overflow accounting. | Ring buffer + overflow metric/event publication. | TASK-FW-002 |
| TASK-FW-004 | WAVE-02-FW-STABILITY | P1 | Define serial ingress/egress arbitration policy for deterministic loop timing. | Scheduler contract documented + enforced in code. | TASK-FW-002, TASK-FW-003 |
| TASK-AR-001 | WAVE-03-ARCH-PORTABILITY | P2 | Introduce `TimeSourceAdapter` and remove direct `millis()` from domain sensors. | Time abstraction interface + migrated `FlowSensor`/`WheelSensor`. | TASK-FW-004 |
| TASK-AR-002 | WAVE-03-ARCH-PORTABILITY | P2 | Refactor monolithic loop into service phases (`Input`, `Control`, `Output`, `Telemetry`). | Phase-oriented orchestration with explicit budgets. | TASK-FW-004 |
| TASK-AR-003 | WAVE-03-ARCH-PORTABILITY | P2 | Consolidate duplicated numeric sanitation/clamp helpers. | Shared utility header + updated call sites. | None |
| TASK-AR-004 | WAVE-03-ARCH-PORTABILITY | P2 | Remove/feature-guard unused protocol tokens/placeholders. | Lean protocol constants + feature-gated placeholders. | TASK-AR-002 |
| TASK-QA-001 | WAVE-04-QUALITY-VALIDATION | P3 | Add board compile matrix (UNO/Nano/Mega) to CI. | CI workflow compile jobs for all declared targets. | TASK-HW-004 |
| TASK-QA-002 | WAVE-04-QUALITY-VALIDATION | P3 | Add unit tests for flow/wheel conversion and controller edge behavior. | Test suite for timeout/clamp/fallback/ramp invariants. | TASK-AR-001 |
| TASK-QA-003 | WAVE-04-QUALITY-VALIDATION | P3 | Add HIL test protocol for ISR pulse rates, serial saturation, and switch bounce. | Repeatable HIL checklist/scripts + pass criteria. | TASK-FW-001, TASK-FW-002 |
| TASK-QA-004 | WAVE-04-QUALITY-VALIDATION | P3 | Add static checks for dead code/include hygiene/protocol drift. | CI static-analysis stage + reporting. | TASK-AR-003, TASK-AR-004 |

## Execution Order

1. WAVE-01-HW-SAFETY
2. WAVE-02-FW-STABILITY
3. WAVE-03-ARCH-PORTABILITY
4. WAVE-04-QUALITY-VALIDATION
