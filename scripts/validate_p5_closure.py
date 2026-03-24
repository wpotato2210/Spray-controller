#!/usr/bin/env python3
"""Deterministic validator for P5 closure artifacts."""

from __future__ import annotations

from pathlib import Path
import re
import sys


CHECKS: tuple[tuple[str, Path, str], ...] = (
    (
        "phase_five_go_decision",
        Path("PHASE_FIVE_READINESS_AUDIT.md"),
        r"\*\*Ready to close P5 \(GO\)\.\*\*",
    ),
    (
        "phase_five_validate_marker_validate_sh",
        Path("PHASE_FIVE_READINESS_AUDIT.md"),
        r"`\./scripts/validate\.sh`",
    ),
    (
        "phase_five_validate_marker_scalability",
        Path("PHASE_FIVE_READINESS_AUDIT.md"),
        r"`python3 scripts/validate_p5_scalability\.py`",
    ),
    (
        "phase_five_validate_marker_closure",
        Path("PHASE_FIVE_READINESS_AUDIT.md"),
        r"`python3 scripts/validate_p5_closure\.py`",
    ),
    (
        "phase_five_descriptor_invariant_contract",
        Path("PHASE_FIVE_READINESS_AUDIT.md"),
        r"descriptor-driven topology and P5 invariants remain frozen and are enforced by deterministic validation coverage",
    ),
    (
        "phase_five_changelog_closure_note",
        Path("PHASE_FIVE_READINESS_AUDIT.md"),
        r"required P5 closure record is now validated directly against\s+`CHANGELOG\.md`",
    ),
    (
        "testing_p5_closure_gate_marker",
        Path("TESTING.md"),
        r"== P5 closure artifact check ==",
    ),
    (
        "changelog_p5_closure_record",
        Path("CHANGELOG.md"),
        r"Executed `P5-CLS-001` by adding deterministic\s+`scripts/validate_p5_closure\.py`",
    ),
    (
        "roadmap_p5_closure_complete",
        Path("ROADMAP_CLOSURE_TASKS.md"),
        r"- \[x\] `P5-CLS-001`",
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

    print("p5_closure_ok")
    return 0


if __name__ == "__main__":
    sys.exit(main())
