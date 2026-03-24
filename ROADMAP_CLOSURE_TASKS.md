# Roadmap Closure Tasks

Legend:

- [ ] pending
- [x] completed

## P0 - Baseline Audit Freeze

- [x] `P0-CLS-001` Add a deterministic loop-path audit to `DEV_NOTES.md`
  covering control-loop entrypoints, execution order, and SEE/THINK/DO
  ownership.
- [x] `P0-CLS-002` Create a frozen interface checklist mapping each
  declaration in `include/interfaces.h` to its implementation file(s).
- [x] `P0-CLS-003` Record baseline assumptions for the current
  single-flow-sensor, single-pump, three-section control model.
- [x] `P0-CLS-004` Re-run and update the P0 readiness audit to mark P0 GO
  once the missing artifacts exist.

## P1 - Pin + Config Finalization

- [x] `P1-CLS-001` Revalidate P1 closure after P0 closes, because current
  roadmap makes P0 a hard prerequisite.

## P2 - Core Control Loop Integration

- [x] `P2-CLS-001` Produce a P2 closure/readiness audit tied to the current
  roadmap scope.
- [x] `P2-CLS-002` Add or record the required bench integration checkpoint
  for run/hold, section-output, and pump-PWM timing.
- [x] `P2-CLS-003` Record P2 closure evidence in `CHANGELOG.md`.

## P3 - Sensor Robustness

- [x] `P3-CLS-001` Preserve current P3 validator coverage and fault-telemetry
  behavior; no blocking closure work is currently open.

## P4 - Operator Interface

- [x] `P4-CLS-001` Preserve current operator reset/menu contract and
  validation coverage; no blocking closure work is currently open.

## P5 - Scalability Preparation

- [x] `P5-CLS-001` Preserve descriptor-driven topology and P5 invariant
  validation; no blocking closure work is currently open.

## P6 - Calibration Workflow

- [x] `P6-CLS-001` Implement operator entrypoints for both flow and wheel
  calibration workflows.
- [ ] `P6-CLS-002` Emit and document deterministic failure signaling when
  calibration storage is invalid, missing, or corrupt and defaults are loaded.
- [ ] `P6-CLS-003` Add an integration checkpoint covering run, save, reboot,
  reload, and override scenarios.
- [ ] `P6-CLS-004` Re-run the P6 audit against the expanded 8-task roadmap
  scope and replace the outdated 4-task closure claim.

## P7 - Documentation Freeze

- [ ] `P7-CLS-001` Add a documentation-freeze closure entry to
  `CHANGELOG.md` with version/date and gate evidence.
- [ ] `P7-CLS-002` Re-run and update the P7 audit so it matches current docs
  and validator results.
- [ ] `P7-CLS-003` Record the final documentation consistency checkpoint
  across pins, fields, units, states, and calibration steps.

## P8 - Release Readiness

- [ ] `P8-CLS-001` Run the full mandatory bench validation matrix.
- [ ] `P8-CLS-002` Run all YAML, protocol, config, and phase-specific
  validators and capture the results.
- [ ] `P8-CLS-003` Verify and, if needed, correct reproducible build/flash
  instructions in `INSTALL.md`.
- [ ] `P8-CLS-004` Align firmware version, config version, docs, changelog,
  and release tag.
- [ ] `P8-CLS-005` Create the release tag and archive known limitations and
  open follow-ups.

## P9 - Optional Enhancements

- [ ] `P9-CLS-001` Define transport, UI, logging, and GPS enhancement specs
  with explicit acceptance criteria.
- [ ] `P9-CLS-002` Define regression gates proving no breakage to the tagged
  single-Nano baseline.

## Priority Order

1. `P0-CLS-001`
2. `P0-CLS-002`
3. `P0-CLS-003`
4. `P0-CLS-004`
5. `P1-CLS-001`
6. `P6-CLS-001`
7. `P6-CLS-002`
8. `P6-CLS-003`
9. `P6-CLS-004`
10. `P7-CLS-001`
11. `P7-CLS-002`
12. `P7-CLS-003`
13. `P2-CLS-001`
14. `P2-CLS-002`
15. `P2-CLS-003`
16. `P8-CLS-001`
17. `P8-CLS-002`
18. `P8-CLS-003`
19. `P8-CLS-004`
20. `P8-CLS-005`
21. `P9-CLS-001`
22. `P9-CLS-002`
