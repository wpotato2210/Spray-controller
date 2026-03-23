#!/usr/bin/env python3
"""Deterministic validator for P1 closure artifacts."""

from __future__ import annotations

import re
from pathlib import Path
import sys


ROOT = Path(__file__).resolve().parents[1]


def require(path: str, pattern: str, label: str) -> None:
    text = (ROOT / path).read_text(encoding="utf-8")
    if not re.search(pattern, text, re.MULTILINE | re.DOTALL):
        print(f"missing:{label}", file=sys.stderr)
        sys.exit(1)


checks = [
    (
        "PHASE_ONE_READINESS.md",
        r"\*\*GO for P1 closure\*\*",
        "phase_one_go_decision",
    ),
    (
        "PHASE_ONE_READINESS.md",
        r"`\./scripts/validate\.sh`\s*\(success:\s*`yamllint_ok`,\s*`protocols_ok`,\s*`config_ok`,\s*`p3_sensor_robustness_ok`,\s*`p0_closure_ok`,\s*`p1_closure_ok`",
        "phase_one_validate_markers",
    ),
    (
        "PHASE_ONE_READINESS.md",
        r"`pins\.yaml`, `config\.yaml`, `CONFIGURATION\.md`, and `HARDWARE\.md` remain aligned",
        "phase_one_alignment_statement",
    ),
    (
        "CHANGELOG.md",
        r"Executed `P1-CLS-001` by adding deterministic\s+`scripts/validate_p1_closure\.py`",
        "changelog_p1_closure_record",
    ),
    (
        "TESTING.md",
        r"== P1 closure artifact check ==",
        "testing_p1_gate_marker",
    ),
]

for path, pattern, label in checks:
    require(path, pattern, label)

print("p1_closure_ok")
