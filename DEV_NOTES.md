# DEV_NOTES.md

Developer Notes

## Standards

- Use modular structure (SEE -> THINK -> DO).
- Maintain frozen interfaces in `include/interfaces.h`.

## Guidelines

- Do not modify module I/O without version update.
- Keep protocols backward compatible.

## P0 baseline audit

### Canonical loop path

- Firmware entrypoint: `setup()` and `loop()` in `Spray-controller.ino`.
- Canonical cadence source: `LOOP_INTERVAL_MS` from `config.h`, enforced by
  `loop()` returning early until `(now_ms - last_loop_ms) >= LOOP_INTERVAL_MS`.
- Canonical validation pairing: roadmap P0 cadence requirement is satisfied by
  the same `LOOP_INTERVAL_MS` constant used by the runtime loop and documented
  configuration assets.

### Execution order ownership map

|Stage|Order in `loop()`|Owner|Frozen I/O boundary|
|---|---|---|---|
|Pre-loop command handling|`processOperatorCommand(now_ms)` -> `g_operator_menu.update(now_ms, kNone)` -> `executeResetIfConfirmed()`|Operator menu + calibration/reset path|Consumes `ME:*` serial input with overlong-line discard-until-newline guard; valid events are processed immediately and may emit `MS:` / `RS:` frames; does not alter frozen sensor/pump method signatures.|
|SEE 1|`readSections()`|`SectionHardwareAdapter` + `SectionManager`|Reads section switches through `SectionHardwareAdapter::readSwitch()` and stores state through `SectionManager::setSection()`.|
|SEE 2|`g_flow_sensor.readFlow()`|`FlowSensor`|Consumes `PulseCounterAdapter`; returns measured flow in L/min.|
|SEE 3|`g_wheel_sensor.readSpeed()`|`WheelSensor`|Consumes `PulseCounterAdapter`; returns speed in km/h.|
|SEE 4|`g_run_hold.readRunHold()`|`RunHoldSwitch`|Consumes `DigitalInputAdapter`; returns run/hold boolean.|
|THINK 1|`g_section_manager.getActiveWidth()` + `getActiveCount()`|`SectionManager`|Derives active width/count from frozen section state.|
|THINK 2|`g_coverage_accumulator.update(...)`|`CoverageAccumulator`|Integrates distance/area from SEE values plus fixed loop interval.|
|THINK 3|`g_flow_controller.computePumpDuty(...)` or `stop()`|`FlowController`|Consumes speed, active width, measured flow; returns one pump PWM duty command.|
|DO 1|`g_pump.setDutyCycle(duty)`|`PumpControl`|Writes PWM through `PwmOutputAdapter::write()`.|
|DO 2|`writeSections()`|`SectionHardwareAdapter`|Writes section outputs through `SectionHardwareAdapter::writeSection()`.|
|DO 3|`publishPreview(...)` / `publishStatus(...)` / `publishSectionTelemetry()` / `publishSensorTelemetry()` / optional `publishPressure(...)`|Protocol publisher helpers in `Spray-controller.ino`|Emits deterministic serial frames without changing control decisions.|

### Baseline assumptions frozen for P0

- One total-flow measurement path: `FlowSensor` backed by one
  `PulseCounterAdapter` instance (`flowPulseCounter()`).
- One global pump command path: `FlowController` computes one duty value and
  `PumpControl` applies it to one PWM output.
- Three binary boom sections: topology is fixed by `SECTION_COUNT == 3` and the
  descriptor table in `pins.h`.
- Run/Hold remains a single boolean gate: HOLD forces `FlowController::stop()`
  and leaves pump output at `PWM_MIN`.
- Pressure sensing is optional telemetry only and does not participate in the
  pump control decision path.
