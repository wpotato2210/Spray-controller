#!/usr/bin/env python3
"""Deterministic validator for W-02 (timing + event service invariants)."""

from __future__ import annotations

from pathlib import Path
import re
import sys

CONFIG_H = Path("config.h")
INTERFACES_H = Path("include/interfaces.h")
ARDUINO_ADAPTERS_H = Path("include/arduino_adapters.h")
ARDUINO_ADAPTERS_CPP = Path("src/arduino_adapters.cpp")
FLOW_SENSOR_CPP = Path("src/flow_sensor.cpp")
WHEEL_SENSOR_CPP = Path("src/wheel_sensor.cpp")
SKETCH = Path("Spray-controller.ino")


def read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def main() -> int:
    errors: list[str] = []

    config = read(CONFIG_H)
    interfaces = read(INTERFACES_H)
    adapters_h = read(ARDUINO_ADAPTERS_H)
    adapters_cpp = read(ARDUINO_ADAPTERS_CPP)
    flow_sensor = read(FLOW_SENSOR_CPP)
    wheel_sensor = read(WHEEL_SENSOR_CPP)
    sketch = read(SKETCH)

    if "struct PulseCounterSnapshot" not in interfaces:
        errors.append("pulse_counter_snapshot_struct_missing")

    if not re.search(r"virtual\s+PulseCounterSnapshot\s+readSnapshot\(\)\s+const\s*=\s*0", interfaces):
        errors.append("pulse_counter_snapshot_interface_missing")

    if "PulseCounterSnapshot readSnapshot() const override;" not in adapters_h:
        errors.append("pulse_counter_snapshot_adapter_override_missing")

    if "PulseCounterSnapshot ArduinoInterruptPulseCounter::readSnapshot() const" not in adapters_cpp:
        errors.append("pulse_counter_snapshot_adapter_impl_missing")

    if "noInterrupts();" not in adapters_cpp or "interrupts();" not in adapters_cpp:
        errors.append("pulse_counter_snapshot_not_atomic")

    if "pulse_counter_.readSnapshot()" not in flow_sensor:
        errors.append("flow_sensor_snapshot_read_missing")

    if "pulse_counter_.readSnapshot()" not in wheel_sensor:
        errors.append("wheel_sensor_snapshot_read_missing")

    if not re.search(r"constexpr\s+uint8_t\s+OPERATOR_EVENT_DEQUEUE_BUDGET_PER_LOOP\s*=\s*[2-9]\d*U", config):
        errors.append("event_dequeue_budget_missing_or_unbounded")

    if not re.search(
        r"while\s*\(processed_events\s*<\s*OPERATOR_EVENT_DEQUEUE_BUDGET_PER_LOOP\)",
        sketch,
    ):
        errors.append("bounded_event_dequeue_loop_missing")

    required_phase_budget_constants = [
        "INPUT_PHASE_BUDGET_MS",
        "CONTROL_PHASE_BUDGET_MS",
        "OUTPUT_PHASE_BUDGET_MS",
        "TELEMETRY_PHASE_BUDGET_MS",
    ]
    for constant in required_phase_budget_constants:
        if constant not in config:
            errors.append(f"{constant.lower()}_missing")

    if "struct PhaseTimingStats" not in sketch:
        errors.append("phase_timing_stats_struct_missing")

    if "updatePhaseOverrunCount(" not in sketch:
        errors.append("phase_overrun_update_function_missing")

    if "PHASE_OVERRUN," not in sketch:
        errors.append("phase_overrun_telemetry_missing")

    if errors:
        for error in sorted(errors):
            print(error)
        return 1

    print("wave_w_02_ok")
    return 0


if __name__ == "__main__":
    sys.exit(main())
