#!/usr/bin/env python3
"""Deterministic validator for P1 closure artifacts."""

from __future__ import annotations

from pathlib import Path
import re
import sys


CHECKS: tuple[tuple[str, Path, str], ...] = (
    (
        "phase_one_go_decision",
        Path("PHASE_ONE_READINESS.md"),
        r"\*\*GO for P1 closure\*\*",
    ),
    (
        "phase_one_validate_markers",
        Path("PHASE_ONE_READINESS.md"),
        r"`\./scripts/validate\.sh`\s*\(success:\s*`yamllint_ok`,\s*`protocols_ok`,\s*`config_ok`,\s*`p3_sensor_robustness_ok`,\s*`p0_closure_ok`,\s*`p1_closure_ok`",
    ),
    (
        "phase_one_alignment_statement",
        Path("PHASE_ONE_READINESS.md"),
        r"`pins\.yaml`, `config\.yaml`, `CONFIGURATION\.md`, and `HARDWARE\.md` remain aligned",
    ),
    (
        "phase_one_changelog_closure_note",
        Path("PHASE_ONE_READINESS.md"),
        r"required P1 closure record is now validated directly against\s+`CHANGELOG\.md`",
    ),
    (
        "changelog_p1_closed_record",
        Path("CHANGELOG.md"),
        r"\*\*0\.7\*\* \(2026-03-22\): P1 closed\.",
    ),
    (
        "changelog_p1_cls_002_record",
        Path("CHANGELOG.md"),
        r"Executed `P1-CLS-002` by extending\s+`scripts/validate_p1_closure\.py`\s+to enforce the required `CHANGELOG\.md`\s+closure record for P1",
    ),
    (
        "testing_p1_gate_marker",
        Path("TESTING.md"),
        r"== P1 closure artifact check ==",
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
        for error in sorted(errors):
            print(error)
        return 1

    print("p1_closure_ok")
    return 0


if __name__ == "__main__":
    sys.exit(main())
