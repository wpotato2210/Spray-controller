# Phase-Five Readiness Audit (P5 Gate Check)

Date: 2026-03-22 (UTC)

Scope: Assess implementation readiness against P5 requirements in
`DEVELOPMENT_ROADMAP.md` and produce a prioritized task list for P5 closure.

## Verdict

**Not ready to close P5 (NO-GO).**

Reason: P5 deterministic tasks and deliverables are not yet fully implemented.
Current firmware still hard-codes section topology, lacks explicit scalability
max constants, does not isolate board I/O adapters from control logic, and does
not expose deterministic section/sensor ID mapping in telemetry.

## Evidence reviewed

- P5 deterministic tasks and deliverables in `DEVELOPMENT_ROADMAP.md`.
- Static compile-time configuration in `config.h`.
- Main loop topology + section I/O wiring in `Spray-controller.ino`.
- Section abstraction in `include/interfaces.h` and `src/section_manager.cpp`.
- Telemetry protocol definitions in `include/protocol.h` and `PROTOCOLS.md`.
- Architecture boundary documentation in `ARCHITECTURE.md`.

## P5 deterministic task status

|P5 task (roadmap)|Status|Findings|
|---|---|---|
|Replace hard-coded section arrays with config-driven descriptors|**Not complete**|Runtime arrays in `Spray-controller.ino` (`kSectionOutputPins`, `kSectionSwitchPins`) are fixed for 3 sections and pin symbols are explicitly enumerated (`PIN_BOOM_1..3`, `PIN_SECTION_SW_1..3`).|
|Introduce compile-time max constants (`MAX_SECTIONS`, `MAX_SENSORS`)|**Not complete**|`config.h` defines `SECTION_COUNT` only; no `MAX_SECTIONS`/`MAX_SENSORS` bound constants are present for scalable static sizing.|
|Isolate hardware adapters from control logic for future multi-MCU bridge|**Complete**|Control-path section, pump, run/hold, pressure, and pulse I/O are routed through adapter interfaces in `include/interfaces.h`, `include/arduino_adapters.h`, and `Spray-controller.ino`; raw GPIO access is confined to Arduino adapter implementations in `src/arduino_adapters.cpp`.|
|Add deterministic ID mapping for section and sensor telemetry|**Not complete**|Protocol currently exposes compact state (`section_mask`, `active_sections`, aggregate values), but does not emit stable explicit section/sensor IDs for scalable consumers.|

## Deliverable status snapshot

|P5 deliverable (roadmap)|Status|Evidence summary|
|---|---|---|
|Section count changes require config updates only|**Not complete**|Section count changes currently require code edits to hard-coded section pin arrays and fixed pin symbol lists in the sketch.|
|Interfaces remain backward compatible for the current 3-section build|**Complete**|The current 3-section build still uses `SECTION_COUNT=3` and descriptor-driven iteration while the control path now depends on frozen adapter interfaces instead of direct GPIO calls.|
|`ARCHITECTURE.md` updated with scalable module boundaries|**Not complete**|Architecture doc still describes current layers but does not define scalable adapter boundaries (e.g., transport abstraction or hardware adapter seams) for P5.|

## Prioritized P5 task list

|Priority|Task ID|Task|Outcome / acceptance signal|
|---|---|---|---|
|P0|P5-TSK-001|Add compile-time scalability bounds in `config.h` (`MAX_SECTIONS`, `MAX_SENSORS`) and rebase `SECTION_COUNT` usage onto bounded static containers.|Build passes with current 3-section config and static assertions enforce `SECTION_COUNT <= MAX_SECTIONS`.|
|P0|P5-TSK-002|Introduce config-driven section descriptors (ID + output pin + switch pin) and refactor section iteration in `Spray-controller.ino` to descriptor loops only.|Changing section count/pin mapping requires config/header updates only (no loop logic rewrite).|
|P0|P5-TSK-003|Define deterministic telemetry ID mapping for sections and sensors (stable IDs, ordering, and field contract) in `PROTOCOLS.md` + `include/protocol.h` and emit from firmware.|Protocol includes explicit ID contract and deterministic output for all configured sections/sensors.|
|P1|P5-TSK-004|Extract board I/O interactions into hardware adapter interfaces (e.g., section input/output adapter, sensor bus adapter), keeping control logic adapter-agnostic.|Main loop/control path no longer calls raw GPIO APIs directly for section/sensor/actuator access.|
|P1|P5-TSK-005|Update `ARCHITECTURE.md` with scalable module boundaries and multi-MCU bridge-ready seams (control core vs adapters vs transport).|Architecture document includes final boundary diagram and dependency direction for scaling.|
|P2|P5-TSK-006|Run/extend deterministic validators to enforce P5 invariants (descriptor count consistency, ID mapping stability, backward-compatible 3-section output).|Validator pass markers are added to `scripts/validate.sh` and documented in `TESTING.md`.|

## Gate decision

- **NO-GO for P5 closure readiness.**
- Execute `P5-TSK-001` through `P5-TSK-006` in priority order, then rerun a
  P5 readiness re-audit for closure.
