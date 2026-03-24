#!/usr/bin/env python3
"""Deterministic validator for P6-CLS-001 closure artifacts."""

from __future__ import annotations

from pathlib import Path
import re
import sys


CHECKS: tuple[tuple[str, Path, str], ...] = (
    (
        "operator_menu_event_entrypoints",
        Path("include/operator_menu.h"),
        r"enum\s+class\s+OperatorMenuEvent[\s\S]*?kFlowCalibrate\s*=\s*5U,[\s\S]*?kWheelCalibrate\s*=\s*6U",
    ),
    (
        "operator_menu_transition_flow_entrypoint",
        Path("src/operator_menu.cpp"),
        r"case\s+OperatorMenuState::kMenu:[\s\S]*?event\s*==\s*OperatorMenuEvent::kFlowCalibrate[\s\S]*?flow_calibration_requested_\s*=\s*true;",
    ),
    (
        "operator_menu_transition_wheel_entrypoint",
        Path("src/operator_menu.cpp"),
        r"case\s+OperatorMenuState::kMenu:[\s\S]*?event\s*==\s*OperatorMenuEvent::kWheelCalibrate[\s\S]*?wheel_calibration_requested_\s*=\s*true;",
    ),
    (
        "operator_command_tokens_entrypoints",
        Path("Spray-controller.ino"),
        r"strcmp\(token,\s*\"FCL\"\)\s*==\s*0[\s\S]*?strcmp\(token,\s*\"WCL\"\)\s*==\s*0",
    ),
    (
        "operator_entrypoint_events_published",
        Path("Spray-controller.ino"),
        r"FLOW_CALIBRATION_ENTRYPOINT[\s\S]*?WHEEL_CALIBRATION_ENTRYPOINT",
    ),
    (
        "testing_p6_closure_gate_marker",
        Path("TESTING.md"),
        r"== P6 closure artifact check ==",
    ),
    (
        "roadmap_p6_cls_001_complete",
        Path("ROADMAP_CLOSURE_TASKS.md"),
        r"- \[x\] `P6-CLS-001`",
    ),
    (
        "changelog_p6_cls_001_record",
        Path("CHANGELOG.md"),
        r"Executed `P6-CLS-001` by adding deterministic\s+`scripts/validate_p6_closure\.py`",
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

    print("p6_closure_ok")
    return 0


if __name__ == "__main__":
    sys.exit(main())
