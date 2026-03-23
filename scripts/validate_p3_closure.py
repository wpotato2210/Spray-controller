#!/usr/bin/env python3
"""Deterministic validator for P3 closure artifacts."""

from __future__ import annotations

from pathlib import Path
import re
import sys


CHECKS: tuple[tuple[str, Path, str], ...] = (
    (
        "phase_three_go_decision",
        Path("PHASE_THREE_READINESS_AUDIT.md"),
        r"\*\*Ready to close P3 \(GO\)\.\*\*",
    ),
    (
        "phase_three_validate_marker_validate_sh",
        Path("PHASE_THREE_READINESS_AUDIT.md"),
        r"`\./scripts/validate\.sh`",
    ),
    (
        "phase_three_validate_marker_sensor_robustness",
        Path("PHASE_THREE_READINESS_AUDIT.md"),
        r"`python3 scripts/validate_p3_sensor_robustness\.py`",
    ),
    (
        "phase_three_validate_marker_closure",
        Path("PHASE_THREE_READINESS_AUDIT.md"),
        r"`python3 scripts/validate_p3_closure\.py`",
    ),
    (
        "phase_three_fault_telemetry_contract",
        Path("PHASE_THREE_READINESS_AUDIT.md"),
        r"Status frame includes `fault_bits` and deterministic `fault_text`",
    ),
    (
        "phase_three_changelog_closure_note",
        Path("PHASE_THREE_READINESS_AUDIT.md"),
        r"required P3 closure record is now validated directly against\s+`CHANGELOG\.md`",
    ),
    (
        "testing_p3_closure_gate_marker",
        Path("TESTING.md"),
        r"== P3 closure artifact check ==",
    ),
    (
        "changelog_p3_closed_record",
        Path("CHANGELOG.md"),
        r"\*\*0\.13\*\* \(2026-03-22\): Re-ran Phase-Three readiness audit",
    ),
    (
        "changelog_p3_cls_001_record",
        Path("CHANGELOG.md"),
        r"Executed `P3-CLS-001` by adding deterministic\s+`scripts/validate_p3_closure\.py`",
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

    print("p3_closure_ok")
    return 0


if __name__ == "__main__":
    sys.exit(main())
