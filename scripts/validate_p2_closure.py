#!/usr/bin/env python3
"""Deterministic validator for P2 closure artifacts."""

from __future__ import annotations

import re
from pathlib import Path
import sys


CHECKS: tuple[tuple[str, Path, str], ...] = (
    (
        "phase_two_go_decision",
        Path("PHASE_TWO_READINESS_AUDIT.md"),
        r"\*\*GO: P2 is ready to close\.\*\*",
    ),
    (
        "phase_two_validate_markers",
        Path("PHASE_TWO_READINESS_AUDIT.md"),
        r"`\./scripts/validate\.sh`\s*\(success:\s*`yamllint_ok`,\s*`protocols_ok`,\s*`config_ok`,\s*`p3_sensor_robustness_ok`,\s*`p0_closure_ok`,\s*`p1_closure_ok`,\s*`p2_closure_ok`",
    ),
    (
        "phase_two_alignment_statement",
        Path("PHASE_TWO_READINESS_AUDIT.md"),
        r"`Spray-controller\.ino` preserves the deterministic SEE -> THINK -> DO ordering with run/hold gating, active-width computation, pump command, and compact `ST:` status telemetry",
    ),
    (
        "phase_two_changelog_closure_note",
        Path("PHASE_TWO_READINESS_AUDIT.md"),
        r"required P2 closure record is now validated directly against\s+`CHANGELOG\.md`",
    ),
    (
        "changelog_p2_closed_record",
        Path("CHANGELOG.md"),
        r"\*\*0\.41\*\* \(2026-03-23\): P2 closed\.",
    ),
    (
        "changelog_p2_cls_002_record",
        Path("CHANGELOG.md"),
        r"Executed `P2-CLS-002` by extending\s+`scripts/validate_p2_closure\.py`\s+to enforce the required `CHANGELOG\.md`\s+closure record for P2",
    ),
    (
        "testing_p2_gate_marker",
        Path("TESTING.md"),
        r"== P2 closure artifact check ==",
    ),
    (
        "changelog_p2_closure_record",
        Path("CHANGELOG.md"),
        r"Executed `P2-CLS-001` by adding deterministic\s+`scripts/validate_p2_closure\.py`",
    ),
    (
        "loop_order_read_then_compute_width",
        Path("Spray-controller.ino"),
        r"spray::readSections\(\);\s+const float measured_flow_lpm = spray::g_flow_sensor\.readFlow\(\);\s+const float speed_kmh = spray::g_wheel_sensor\.readSpeed\(\);\s+const bool run_enabled = spray::g_run_hold\.readRunHold\(\);\s+const float active_width_m = spray::g_section_manager\.getActiveWidth\(\);",
    ),
    (
        "hold_forces_safe_pump_state",
        Path("Spray-controller.ino"),
        r"uint8_t duty = spray::PWM_MIN;\s+if \(run_enabled\) \{\s+duty = spray::g_flow_controller\.computePumpDuty\(speed_kmh, active_width_m, measured_flow_lpm\);\s+\} else \{\s+spray::g_flow_controller\.stop\(\);\s+\}",
    ),
    (
        "compact_status_publish",
        Path("Spray-controller.ino"),
        r"spray::publishStatus\(measured_flow_lpm, duty, run_enabled, spray::getStatusFaultBitfield\(\)\);",
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

    print("p2_closure_ok")
    return 0


if __name__ == "__main__":
    sys.exit(main())
