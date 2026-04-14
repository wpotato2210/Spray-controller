# Spray Controller Change Log

## Versions

- **0.60** (2026-04-14): Executed `W-04` closure gate by adding
  `scripts/validate_wave_w_04.py` to enforce Wave-04 validation invariants
  (WAVE-04 static + native checks wired in canonical validation,
  CI wave jobs + AVR board matrix preserved, and required HIL protocol
  artifact/scenarios present), and wiring the gate into
  `./scripts/validate.sh`, `TESTING.md`, and validation workflow path triggers.

- **0.59** (2026-04-14): Executed `W-03` closure gate by adding
  `scripts/validate_wave_w_03.py` to enforce Wave-03 HAL/fault invariants
  (display backend abstraction with compile-time `ST7920_SPI`/`NONE`
  selection, centralized `FaultManager` latching/inhibit integration, and
  direct index-based section descriptor access checks), wiring the new gate
  into `./scripts/validate.sh` and `TESTING.md`, and aligning section write/
  bitmask paths to indexed descriptor access.

- **0.58** (2026-04-14): Executed `W-02` closure gate by adding
  `scripts/validate_wave_w_02.py` to enforce Wave-02 timing/event invariants
  (atomic pulse snapshot interface + adapter implementation, bounded
  per-loop operator event dequeue, and phase budget/overrun telemetry
  coverage), and wiring the gate into `./scripts/validate.sh` and
  `TESTING.md`.

- **0.57** (2026-04-14): Executed `W-01` closure gate by adding
  `scripts/validate_wave_w_01.py` to enforce the Wave-01 invariants
  (AVR ST7920 page-buffer rendering, assigned-role pin policy guard coverage,
  and explicit `U8g2` dependency documentation in `README.md` and
  `INSTALL.md`), and wiring the gate into `./scripts/validate.sh` and
  `TESTING.md`.

- **0.56** (2026-04-12): Executed `WAVE-02-TIMING-EVENTS` by adding an
  atomic `PulseCounterSnapshot` API and wiring flow/wheel reads to single-lock
  snapshots, increasing menu event dequeue service to a bounded per-loop budget
  (`OPERATOR_EVENT_DEQUEUE_BUDGET_PER_LOOP`), and introducing loop phase timing
  budgets with cumulative overrun telemetry events
  (`RS:PHASE_OVERRUN,<phase>,<count>`).

- **0.55** (2026-04-12): Executed `WAVE-01-MEM-PIN` by moving AVR ST7920
  rendering from full-frame (`_F_`) U8g2 mode to page-buffer (`_1_`) mode,
  expanding compile-time pin policy guards so all assigned role pins are checked
  against reserved LCD/SPI pins, and aligning `README.md`/`INSTALL.md` with the
  required `U8g2` dependency + AVR SRAM guidance.

- **0.54** (2026-04-12): Executed `WAVE-02-FW-STABILITY` by adding
  `INPUT_DEBOUNCE_MS`-backed run/hold + section switch debounce in the loop
  path, introducing bounded serial ingress/event-queue handling with
  overflow telemetry (`RS:OP_EVENT_OVERFLOW,<count>`), implementing
  deterministic telemetry frame budgeting via cursor scheduling, adding
  `scripts/validate_fw_wave_02.py`, and wiring the new gate into
  `./scripts/validate.sh` + `TESTING.md`.

- **0.53** (2026-04-12): Executed `WAVE-01-HW-SAFETY` by remapping Mega
  `PIN_BTN_SELECT` off `LCD_RESET`, adding compile-time
  `include/pin_policy_guards.h` uniqueness/reserved-domain assertions,
  introducing deterministic `scripts/validate_pin_wave_004.py` hardware-doc
  parity checks, and integrating the new check into `./scripts/validate.sh`
  and the validation workflow trigger matrix.

- **0.52** (2026-04-08): Executed `PIN-WAVE-003` by adding deterministic
  `scripts/validate_pin_wave_003.py` Mega-profile policy checks for
  `pins.yaml` invariants, Uno/Nano selector guardrails, and hardware policy
  documentation continuity; integrated the check into `./scripts/validate.sh`
  and documented command + pass marker in `TESTING.md`.

- **0.51** (2026-04-08): Executed `PIN-WAVE-002` by adding deterministic
  `scripts/validate_pin_wave_002.py` parity coverage across `pins.yaml`,
  Uno/Nano pin headers, and target-policy docs; integrated the check into
  `./scripts/validate.sh` and documented command + pass marker in `TESTING.md`.

- **0.50** (2026-04-08): Executed `PIN-WAVE-001` by adding deterministic
  `scripts/validate_pin_wave_001.py` pin-map invariants coverage, integrating
  the check into `./scripts/validate.sh`, and documenting command + pass marker
  in `TESTING.md`.

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
  `config_ok`), and recorded pass artifact `validation/validation_pass.txt`.

- **0.8** (2026-03-22): Executed `P1-GATE-005` readiness re-audit; confirmed
  `P1-GATE-001` through `P1-GATE-004` are complete and recorded **GO** decision
  to proceed to P2 in `PHASE_TWO_READINESS_AUDIT.md`.

- **0.9** (2026-03-22): Completed P3 readiness audit in
  `PHASE_THREE_READINESS_AUDIT.md`; decision: **NO-GO for P3 closure** pending
  blocker tasks `P3-TSK-001` through `P3-TSK-003`.

- **0.10** (2026-03-22): Executed `P3-TSK-002` by adding explicit flow
  stale-timeout fallback in `FlowSensor::readFlow()` using
  `FLOW_STALE_TIMEOUT_MS` + bounded fallback output `FLOW_STALE_FALLBACK_LPM`
  (default `0.0`), while preserving burst clamp behavior via `MAX_FLOW_LPM`.
- **0.11** (2026-03-22): Executed `P3-TSK-003` by extending `ST` telemetry with
  deterministic fault visibility fields (`fault_bits`, `fault_text`) and wiring
  fault source bits for flow stale timeout, wheel timeout, and sensor-config
  validity.
- **0.12** (2026-03-22): Executed `P3-TSK-004` by adding compile-time gated
  pressure telemetry abstraction (`PressureSensor::readPressure()`),
  default-disabled gate (`ENABLE_PRESSURE_SENSOR=false`), optional
  `PR:<pressure_kpa>` output, and pressure config fault bit wiring when enabled.

- **0.13** (2026-03-22): Re-ran Phase-Three readiness audit with deterministic
  checks (`./scripts/validate.sh`, `python3
  scripts/validate_p3_sensor_robustness.py`), confirmed all P3 deliverables
  complete, and recorded **GO** decision in `PHASE_THREE_READINESS_AUDIT.md`.

- **0.14** (2026-03-22): Executed Phase-Zero readiness audit in
  `PHASE_ZERO_READINESS_AUDIT.md`; decision: **NO-GO for P0 closure** pending
  missing deliverables for DEV_NOTES loop ownership mapping and a frozen
  interface checklist mapped to source implementations.

- **0.15** (2026-03-22): Executed Phase-Four readiness audit in
  `PHASE_FOUR_READINESS_AUDIT.md`; decision: **NO-GO for P4 closure readiness**
  with prioritized task backlog `P4-TSK-001` through `P4-TSK-007`.

- **0.16** (2026-03-22): Executed `P4-TSK-001` by defining the
  deterministic operator-interface contract in `PROTOCOLS.md` and `USAGE.md`,
  including menu states/transitions, required `PV` preview payload fields
  (`speed_kmh,flow_lpm,pump_duty,active_sections,distance_m,area_ha`), fixed
  preview cadence rules, and explicit reset-confirm handshake semantics.

- **0.17** (2026-03-22): Executed `P4-TSK-003` by implementing fixed-cadence
  `PV` preview publishing in the main loop with
  `speed_kmh,flow_lpm,pump_duty,active_sections,distance_m,area_ha` payload
  order and deterministic protocol prefix wiring (`MSG_PREVIEW_PREFIX`).

- **0.18** (2026-03-22): Executed `P4-TSK-005` by implementing explicit
  two-step reset confirmation execution in firmware (`ME:*` input events to
  `RESET_CONFIRM` + `CFM`), deterministic reset action for counters and runtime
  calibration/sensor reset hooks, and observable reset/state telemetry frames
  (`MS:` and `RS:COUNTERS_CALIBRATION_RESET`).

- **0.19** (2026-03-22): Executed `P4-TSK-006` by adding deterministic
  P4 validation script `scripts/validate_p4_operator_interface.py`,
  integrating it into canonical `./scripts/validate.sh` gate checks,
  and documenting validator command/pass marker plus enforced
  scenario coverage in `TESTING.md`.

- **0.20** (2026-03-22): Executed `P4-TSK-007` by replacing
  `DIAGRAMS.md` placeholders with implemented deterministic diagrams for
  SEE/THINK/DO architecture, operator menu transitions, preview/reset I/O
  dependencies, and logical wiring aligned to `ME`/`ST`/`PV`/`MS`/`RS`
  protocol behavior.

- **0.21** (2026-03-22): Re-ran Phase-Four readiness audit in
  `PHASE_FOUR_READINESS_AUDIT.md`; confirmed all P4 deterministic tasks and
  deliverables are complete, and recorded **GO** decision to proceed to P5.

- **0.22** (2026-03-22): Executed Phase-Five readiness audit in
  `PHASE_FIVE_READINESS_AUDIT.md`; decision: **NO-GO for P5 closure readiness**
  with prioritized task backlog `P5-TSK-001` through `P5-TSK-006`.

- **0.24** (2026-03-23): Executed `P5-TSK-003` by formalizing
  deterministic section/sensor telemetry contracts in `include/protocol.h`,
  routing firmware emission through ordered telemetry contract tables, and
  documenting the stable ID mapping rules in `PROTOCOLS.md`.
- **0.25** (2026-03-23): Executed `P5-TSK-004` by tightening the
  hardware adapter boundary: section switch/output access is now cached
  behind `ArduinoSectionHardwareAdapter`, and Phase Five audit evidence
  now records the control path as adapter-driven instead of direct GPIO.

- **0.26** (2026-03-23): Executed `P5-TSK-005` by finalizing
  `ARCHITECTURE.md` with scalable control-core, frozen-adapter, and
  concrete-transport boundaries, explicit dependency direction, and
  multi-MCU bridge-ready seams for future distributed I/O.

- **0.27** (2026-03-23): Executed `P5-TSK-006` by adding
  `scripts/validate_p5_scalability.py`, wiring the new P5 invariant check
  into `./scripts/validate.sh`, and documenting the required pass markers
  plus enforced descriptor/ID/backward-compatibility invariants in
  `TESTING.md`.

- **0.28** (2026-03-23): Re-ran Phase-Five readiness audit in
  `PHASE_FIVE_READINESS_AUDIT.md`; confirmed all P5 deterministic tasks and
  deliverables are complete, validated the canonical gate with
  `./scripts/validate.sh`, and recorded **GO** decision to proceed to P6.

- **0.29** (2026-03-23): Executed `P6-TSK-001` by replacing the
  flow-calibration placeholder in `CALIBRATION.md` with a deterministic
  5-step procedure, adding `scripts/validate_p6_flow_calibration.py`, and
  wiring the new validator into `./scripts/validate.sh` and `TESTING.md`.

- **0.30** (2026-03-23): Executed `P6-TSK-002` by replacing the
  wheel-calibration placeholder in `CALIBRATION.md` with a deterministic
  5-step distance-pulse procedure, adding
  `scripts/validate_p6_wheel_calibration.py`, and wiring the new validator
  into `./scripts/validate.sh` and `TESTING.md`.

## Placeholders

- Update dates.
- Notes for future releases and bug fixes.

- **0.31** (2026-03-23): Executed `P6-TSK-003` by adding EEPROM-backed
  calibration persistence with a validity marker and checksum, loading
  defaults on invalid storage at boot, routing flow/wheel conversions through
  the active calibration profile, and wiring
  `scripts/validate_p6_calibration_storage.py` into `./scripts/validate.sh`
  and `TESTING.md`.

- **0.32** (2026-03-23): Executed `P6-TSK-004` by persisting an
  operator `use_defaults_override` flag in the calibration store, routing
  active calibration accessors back to `config.h` defaults when the flag is
  enabled, and documenting plus validating the defaults-vs-calibrated
  override workflow.

- **0.33** (2026-03-23): Executed `P7-TSK-001` by aligning
  `PROTOCOLS.md`, `USAGE.md`, and `TESTING.md` with the implemented firmware
  contract, adding a deterministic `scripts/validate_p7_documentation.py`
  check to `./scripts/validate.sh`, and documenting the current cadence,
  reset, pin-map, and control-constant behavior for the frozen doc set.
- **0.33** (2026-03-23): Executed Phase-Six readiness audit in
  `PHASE_SIX_READINESS_AUDIT.md`; confirmed all P6 deterministic tasks and
  deliverables are complete, validated the canonical gate with
  `./scripts/validate.sh`, and recorded **GO** decision to close P6 and
  proceed to P7.

- **0.35** (2026-03-23): Executed `P0-CLS-001` by replacing the
  Phase-Zero baseline placeholders with a canonical SEE/THINK/DO loop-path
  audit in `DEV_NOTES.md`, converting `include_path_audit.md` into a frozen
  interface checklist mapped to implementation owners, and re-running the P0
  audit to record a **GO** closure decision in
  `PHASE_ZERO_READINESS_AUDIT.md`.

- **0.36** (2026-03-23): Executed `P0-CLS-002` by adding deterministic
  `scripts/validate_p0_closure.py` checks for P0 roadmap-path, cadence,
  interface-owner, and baseline-assumption invariants, wiring that validator
  into `./scripts/validate.sh`, and refreshing `PHASE_ZERO_READINESS_AUDIT.md`
  with the 2026-03-23 validation evidence set.

- **0.37** (2026-03-23): Executed `P0-CLS-003` by extending
  `scripts/validate_p0_closure.py` to enforce the required `CHANGELOG.md`
  closure record for P0, refreshing `PHASE_ZERO_READINESS_AUDIT.md` to cite
  that deterministic closure evidence, and preserving the existing **GO**
  closure decision for Phase Zero.

- **0.38** (2026-03-23): Executed `P0-CLS-004` by extending
  `scripts/validate_p0_closure.py` to enforce the required P0 audit evidence
  and `CHANGELOG.md` closure invariants together, refreshing
  `PHASE_ZERO_READINESS_AUDIT.md` to document that combined deterministic gate,
  and preserving the existing **GO** closure decision for Phase Zero.

- **0.39** (2026-03-23): Executed `P1-CLS-001` by adding deterministic
  `scripts/validate_p1_closure.py` checks for the P1 audit verdict, canonical
  validation markers, doc/config alignment statement, and `CHANGELOG.md`
  closure evidence, wiring that validator into `./scripts/validate.sh`, and
  refreshing `PHASE_ONE_READINESS.md` plus `TESTING.md` with the current
  deterministic P1 closure gate.

- **0.40** (2026-03-23): Executed `P2-CLS-001` by adding deterministic
  `scripts/validate_p2_closure.py` checks for the P2 audit verdict, canonical
  validation markers, fixed SEE/THINK/DO loop ordering, HOLD-safe pump gating,
  and compact `ST:` status publish evidence, wiring that validator into
  `./scripts/validate.sh`, and refreshing `PHASE_TWO_READINESS_AUDIT.md` plus
  `TESTING.md` with the current deterministic P2 closure gate.
- **0.40** (2026-03-23): Executed `P1-CLS-002` by extending
  `scripts/validate_p1_closure.py` to enforce the required `CHANGELOG.md`
  closure record for P1, refreshing `PHASE_ONE_READINESS.md` to cite that
  deterministic closure evidence, and preserving the existing **GO** closure
  decision for Phase One.

- **0.34** (2026-03-23): Executed Phase-Seven readiness audit in
  `PHASE_SEVEN_READINESS_AUDIT.md`; decision: **NO-GO for P7 closure
  readiness** pending documentation-freeze tasks `P7-TSK-001` through
  `P7-TSK-004`.

- **0.41** (2026-03-23): P2 closed. Integrated deterministic SEE -> THINK ->
  DO control-cycle behavior, validated with `./scripts/validate.sh` (success
  markers: `yamllint_ok`, `protocols_ok`, `config_ok`,
  `p3_sensor_robustness_ok`, `p0_closure_ok`, `p1_closure_ok`,
  `p2_closure_ok`), and recorded the Phase-Two **GO** closure decision in
  `PHASE_TWO_READINESS_AUDIT.md`.

- **0.42** (2026-03-23): Executed `P2-CLS-002` by extending
  `scripts/validate_p2_closure.py` to enforce the required `CHANGELOG.md`
  closure record for P2, refreshing `PHASE_TWO_READINESS_AUDIT.md` to cite that
  deterministic closure evidence, and preserving the existing **GO** closure
  decision for Phase Two.

- **0.43** (2026-03-23): Executed `P2-CLS-003` by recording the final P2
  closure evidence checkpoint in `CHANGELOG.md` and marking the roadmap closure
  tracker complete for `P2-CLS-003`.

- **0.44** (2026-03-23): Executed `P3-CLS-001` by adding deterministic
  `scripts/validate_p3_closure.py` checks for the Phase-Three **GO** verdict,
  required fault-telemetry evidence (`fault_bits`, `fault_text`), and
  `CHANGELOG.md` closure record coverage; wired the new P3 closure gate into
  `./scripts/validate.sh`, refreshed `PHASE_THREE_READINESS_AUDIT.md` and
  `TESTING.md` with `p3_closure_ok`, and marked `ROADMAP_CLOSURE_TASKS.md`
  complete for `P3-CLS-001`.

- **0.45** (2026-03-23): Executed `P4-CLS-001` by adding deterministic
  `scripts/validate_p4_closure.py` checks for the Phase-Four **GO** verdict,
  frozen operator reset/menu contract coverage, and `CHANGELOG.md` closure
  record evidence; wired the new P4 closure gate into `./scripts/validate.sh`,
  refreshed `PHASE_FOUR_READINESS_AUDIT.md` and `TESTING.md` with
  `p4_closure_ok`, and marked `ROADMAP_CLOSURE_TASKS.md` complete for
  `P4-CLS-001`.

- **0.46** (2026-03-24): Executed `P5-CLS-001` by adding deterministic
  `scripts/validate_p5_closure.py` checks for the Phase-Five **GO** verdict,
  frozen descriptor-driven topology and P5 invariant coverage, and
  `CHANGELOG.md` closure record evidence; wired the new P5 closure gate into
  `./scripts/validate.sh`, refreshed `PHASE_FIVE_READINESS_AUDIT.md` and
  `TESTING.md` with `p5_closure_ok`, and marked `ROADMAP_CLOSURE_TASKS.md`
  complete for `P5-CLS-001`.

- **0.47** (2026-03-24): Executed `P6-CLS-001` by adding deterministic
  `scripts/validate_p6_closure.py` checks for operator entrypoint coverage of
  flow and wheel calibration workflows (`ME:FCL` and `ME:WCL`), wiring the new
  P6 closure gate into `./scripts/validate.sh`, refreshing `TESTING.md` with
  `p6_closure_ok`, and marking `ROADMAP_CLOSURE_TASKS.md` complete for
  `P6-CLS-001`.

- **0.48** (2026-03-24): Re-ran the Phase-Seven readiness audit in
  `PHASE_SEVEN_READINESS_AUDIT.md`, confirmed documentation-freeze gate
  completion against current deterministic validators (`p7_documentation_ok`
  in `./scripts/validate.sh`), and recorded a **GO** decision to close P7.

- **0.49** (2026-03-24): Executed the Phase-Eight readiness audit in
  `PHASE_EIGHT_READINESS_AUDIT.md`; decision: **NO-GO for P8 closure
  readiness** pending prioritized release tasks `P8-RA-001` through
  `P8-RA-007`.

- **0.50** (2026-04-11): Revalidated `P3-CLS-001` deterministically by running
  `python3 scripts/validate_p3_closure.py` and `python3
  scripts/validate_p3_sensor_robustness.py`; both checks passed and preserved
  the existing P3 closure/fault-telemetry contract without interface changes.
