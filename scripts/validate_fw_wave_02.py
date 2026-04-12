#!/usr/bin/env python3
"""Deterministic validator for WAVE-02-FW-STABILITY artifacts."""

from __future__ import annotations

from pathlib import Path
import re
import sys

CHECKS: tuple[tuple[str, Path, str], ...] = (
    (
        "config_wave_02_constants",
        Path("config.h"),
        r"INPUT_DEBOUNCE_MS[\s\S]*TELEMETRY_FRAME_BUDGET_PER_LOOP[\s\S]*SERIAL_INGRESS_BUDGET_BYTES_PER_LOOP[\s\S]*OPERATOR_EVENT_QUEUE_CAPACITY",
    ),
    (
        "debounced_section_and_runhold_loop_path",
        Path("Spray-controller.ino"),
        r"readSections\(uint32_t now_ms\)[\s\S]*updateDebouncedState[\s\S]*readRunHoldDebounced\(uint32_t now_ms\)",
    ),
    (
        "serial_ingress_bounded_budget",
        Path("Spray-controller.ino"),
        r"consumed_bytes\s*<\s*SERIAL_INGRESS_BUDGET_BYTES_PER_LOOP",
    ),
    (
        "operator_event_queue_overflow",
        Path("Spray-controller.ino"),
        r"struct\s+OperatorEventQueue[\s\S]*overflow_count[\s\S]*publishOperatorOverflowEvent\(uint32_t overflow_count\)[\s\S]*OP_EVENT_OVERFLOW",
    ),
    (
        "telemetry_frame_budget_scheduler",
        Path("Spray-controller.ino"),
        r"struct\s+TelemetryCursor[\s\S]*emitNextTelemetryFrame\([\s\S]*TELEMETRY_FRAME_BUDGET_PER_LOOP",
    ),
    (
        "validation_gate_integration",
        Path("scripts/validate.sh"),
        r"WAVE-02 firmware stability check[\s\S]*validate_fw_wave_02\.py",
    ),
    (
        "testing_marker",
        Path("TESTING.md"),
        r"fw_wave_02_stability_ok",
    ),
    (
        "changelog_wave_entry",
        Path("CHANGELOG.md"),
        r"Executed `WAVE-02-FW-STABILITY`",
    ),
)


def main() -> int:
    errors: list[str] = []
    cache: dict[Path, str] = {}

    for name, path, pattern in CHECKS:
        if path not in cache:
            cache[path] = path.read_text(encoding="utf-8")
        if re.search(pattern, cache[path], flags=re.MULTILINE | re.DOTALL) is None:
            errors.append(f"missing:{name}")

    if errors:
        for error in sorted(errors):
            print(error)
        return 1

    print("fw_wave_02_stability_ok")
    return 0


if __name__ == "__main__":
    sys.exit(main())
