#!/usr/bin/env python3
"""Deterministic validator for the second P6 wheel calibration workflow task."""

from __future__ import annotations

from pathlib import Path
import re
import sys


CHECKS: tuple[tuple[str, str], ...] = (
    ("wheel_sequence_heading", r"^## Wheel Calibration$"),
    (
        "wheel_step_1",
        r"1\. Reset any prior wheel-distance override to the default wheel geometry\s+constants\.",
    ),
    (
        "wheel_step_2",
        r"2\. Mark a straight test course of known length \(minimum 50 m recommended\)\s+and set tire pressure to the normal operating value before moving\.",
    ),
    (
        "wheel_step_3",
        r"3\. Drive the wheel sensor over the full measured course while recording\s+total wheel pulses and reject runs with zero pulses or wheel slip\.",
    ),
    (
        "wheel_step_4",
        r"4\. Compute `wheel_distance_per_pulse_m = measured_distance_m /\s+total_wheel_pulses` and `wheel_pulses_per_100m = \(total_wheel_pulses /\s+measured_distance_m\) \* 100\.0`\.",
    ),
    (
        "wheel_step_5",
        r"5\. Accept the calibration only when `0\.02 <= wheel_distance_per_pulse_m <=\s+0\.5` and repeated runs agree within 5%\.",
    ),
    (
        "wheel_default_constants",
        r"- Baseline defaults: `WHEEL_CIRCUMFERENCE_M = 2\.0`,\s+`WHEEL_PULSES_PER_REV = 20\.0`\.",
    ),
    (
        "wheel_derived_default",
        r"- Derived default: `wheel_distance_per_pulse_m = 0\.1`\.",
    ),
    (
        "wheel_persistence_note",
        r"- Persistence hand-off: until P6 storage is implemented, update `config\.h`\s+so `WHEEL_CIRCUMFERENCE_M / WHEEL_PULSES_PER_REV` matches the accepted\s+wheel distance per pulse before the next run\.",
    ),
)


def main() -> int:
    content = Path("CALIBRATION.md").read_text(encoding="utf-8")
    errors = [
        f"missing:{name}"
        for name, pattern in CHECKS
        if re.search(pattern, content, flags=re.MULTILINE) is None
    ]
    if errors:
        for error in errors:
            print(error)
        return 1

    print("p6_wheel_calibration_ok")
    return 0


if __name__ == "__main__":
    sys.exit(main())
