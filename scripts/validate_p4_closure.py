#!/usr/bin/env python3
"""Deterministic validator for P4 closure artifacts."""

from __future__ import annotations

from pathlib import Path
import re
import sys


CHECKS: tuple[tuple[str, Path, str], ...] = (
    (
        "phase_four_go_decision",
        Path("PHASE_FOUR_READINESS_AUDIT.md"),
        r"\*\*Ready to close P4 \(GO\)\.\*\*",
    ),
    (
        "phase_four_validate_marker_validate_sh",
        Path("PHASE_FOUR_READINESS_AUDIT.md"),
        r"`\./scripts/validate\.sh`",
    ),
    (
        "phase_four_validate_marker_operator_interface",
        Path("PHASE_FOUR_READINESS_AUDIT.md"),
        r"`python3 scripts/validate_p4_operator_interface\.py`",
    ),
    (
        "phase_four_validate_marker_closure",
        Path("PHASE_FOUR_READINESS_AUDIT.md"),
        r"`python3 scripts/validate_p4_closure\.py`",
    ),
    (
        "phase_four_operator_reset_menu_contract",
        Path("PHASE_FOUR_READINESS_AUDIT.md"),
        r"operator reset/menu contract remains frozen and is enforced by deterministic validation coverage",
    ),
    (
        "phase_four_changelog_closure_note",
        Path("PHASE_FOUR_READINESS_AUDIT.md"),
        r"required P4 closure record is now validated directly against\s+`CHANGELOG\.md`",
    ),
    (
        "testing_p4_closure_gate_marker",
        Path("TESTING.md"),
        r"== P4 closure artifact check ==",
    ),
    (
        "changelog_p4_closure_record",
        Path("CHANGELOG.md"),
        r"Executed `P4-CLS-001` by adding deterministic\s+`scripts/validate_p4_closure\.py`",
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

    print("p4_closure_ok")
    return 0


if __name__ == "__main__":
    sys.exit(main())
