#!/usr/bin/env python3
"""Deterministic P3 sensor robustness validator."""

from __future__ import annotations

from pathlib import Path
import re
import sys


CHECKS: tuple[tuple[str, Path, str], ...] = (
    (
        "wheel_timeout_constant",
        Path("config.h"),
        r"constexpr\s+uint32_t\s+WHEEL_PULSE_TIMEOUT_MS\s*=\s*\d+UL;",
    ),
    (
        "flow_stale_timeout_constant",
        Path("config.h"),
        r"constexpr\s+uint32_t\s+FLOW_STALE_TIMEOUT_MS\s*=\s*SENSOR_FAULT_TIMEOUT_MS;",
    ),
    (
        "flow_stale_fallback_constant",
        Path("config.h"),
        r"constexpr\s+float\s+FLOW_STALE_FALLBACK_LPM\s*=\s*0\.0f;",
    ),
    (
        "pressure_default_disabled",
        Path("config.h"),
        r"constexpr\s+bool\s+ENABLE_PRESSURE_SENSOR\s*=\s*false;",
    ),
    (
        "wheel_timeout_logic",
        Path("src/wheel_sensor.cpp"),
        r"pulse_timed_out\s*=\s*\(now_ms\s*-\s*last_pulse_ms\)\s*>=\s*WHEEL_PULSE_TIMEOUT_MS",
    ),
    (
        "wheel_timeout_zero_speed",
        Path("src/wheel_sensor.cpp"),
        r"if\s*\(config_fault_active_\s*\|\|\s*pulse_timed_out\)\s*\{[\s\S]*?return\s+0\.0f;",
    ),
    (
        "flow_timeout_logic",
        Path("src/flow_sensor.cpp"),
        r"pulse_timed_out\s*=\s*\(now_ms\s*-\s*last_pulse_ms\)\s*>=\s*FLOW_STALE_TIMEOUT_MS",
    ),
    (
        "flow_timeout_fallback",
        Path("src/flow_sensor.cpp"),
        r"if\s*\(pulse_timed_out\)\s*\{[\s\S]*?return\s+FLOW_STALE_FALLBACK_LPM;",
    ),
    (
        "flow_clamp_max",
        Path("src/flow_sensor.cpp"),
        r"if\s*\(flow_lpm\s*>\s*MAX_FLOW_LPM\)\s*\{[\s\S]*?flow_lpm\s*=\s*MAX_FLOW_LPM;",
    ),
    (
        "pressure_guard_declaration",
        Path("Spray-controller.ino"),
        r"#if\s+ENABLE_PRESSURE_SENSOR[\s\S]*?PressureSensor\s+g_pressure_sensor",
    ),
    (
        "pressure_guard_publish",
        Path("Spray-controller.ino"),
        r"#if\s+ENABLE_PRESSURE_SENSOR[\s\S]*?publishPressure\(spray::g_pressure_sensor\.readPressure\(\)\);",
    ),
    (
        "status_fault_bits_and_text",
        Path("PROTOCOLS.md"),
        r"`ST:<lpm>,<pump_duty>,<run>,<section_mask>,<fault_bits>,<fault_text>\\n`",
    ),
)


def main() -> int:
    errors: list[str] = []
    cache: dict[Path, str] = {}

    for name, path, pattern in CHECKS:
        if path not in cache:
            cache[path] = path.read_text(encoding="utf-8")
        if re.search(pattern, cache[path], flags=re.MULTILINE) is None:
            errors.append(f"missing:{name}")

    if errors:
        for err in sorted(errors):
            print(err)
        return 1

    print("p3_sensor_robustness_ok")
    return 0


if __name__ == "__main__":
    sys.exit(main())
