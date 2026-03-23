#!/usr/bin/env python3
"""Deterministic validator for the first P6 flow calibration workflow task."""

from __future__ import annotations

from pathlib import Path
import re
import sys


CHECKS: tuple[tuple[str, str], ...] = (
    ("flow_sequence_heading", r"^## Flow Sensor Calibration$"),
    (
        "flow_step_1",
        r"1\. Reset calibration override to defaults or clear prior calibrated flow\s+constant\.",
    ),
    (
        "flow_step_2",
        r"2\. Prime the plumbing with clean water and run the pump at a fixed PWM\s+duty until flow stabilizes\.",
    ),
    (
        "flow_step_3",
        r"3\. Collect pulses and measured liters over a bounded sample window\s+\(minimum 10 L recommended\)\.",
    ),
    (
        "flow_step_4",
        r"4\. Compute `flow_pulses_per_liter = total_pulses / measured_liters` and\s+reject zero or negative results\.",
    ),
    (
        "flow_step_5",
        r"5\. Accept the calibration only when the recomputed factor is within a\s+documented sanity window and repeated runs agree within 5%\.",
    ),
    (
        "flow_sanity_window",
        r"- Sanity window: accept only `300\.0 <= flow_pulses_per_liter <= 700\.0`\s+for the YF-S201C baseline build\.",
    ),
    (
        "flow_persistence_note",
        r"- Persistence hand-off: until P6 storage is implemented, record the\s+accepted factor in `config\.h` as `FLOW_PULSES_PER_LITER` before the\s+next run\.",
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

    print("p6_flow_calibration_ok")
    return 0


if __name__ == "__main__":
    sys.exit(main())
