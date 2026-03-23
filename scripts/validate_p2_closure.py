#!/usr/bin/env python3
"""Deterministic validator for P2 closure artifacts."""

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
        "PHASE_TWO_READINESS_AUDIT.md",
        r"\*\*GO: P2 is ready to close\.\*\*",
        "phase_two_go_decision",
    ),
    (
        "PHASE_TWO_READINESS_AUDIT.md",
        r"`\./scripts/validate\.sh`\s*\(success:\s*`yamllint_ok`,\s*`protocols_ok`,\s*`config_ok`,\s*`p3_sensor_robustness_ok`,\s*`p0_closure_ok`,\s*`p1_closure_ok`,\s*`p2_closure_ok`",
        "phase_two_validate_markers",
    ),
    (
        "PHASE_TWO_READINESS_AUDIT.md",
        r"`Spray-controller\.ino` preserves the deterministic SEE -> THINK -> DO ordering with run/hold gating, active-width computation, pump command, and compact `ST:` status telemetry",
        "phase_two_alignment_statement",
    ),
    (
        "TESTING.md",
        r"== P2 closure artifact check ==",
        "testing_p2_gate_marker",
    ),
    (
        "CHANGELOG.md",
        r"Executed `P2-CLS-001` by adding deterministic\s+`scripts/validate_p2_closure\.py`",
        "changelog_p2_closure_record",
    ),
    (
        "Spray-controller.ino",
        r"spray::readSections\(\);\s+const float measured_flow_lpm = spray::g_flow_sensor\.readFlow\(\);\s+const float speed_kmh = spray::g_wheel_sensor\.readSpeed\(\);\s+const bool run_enabled = spray::g_run_hold\.readRunHold\(\);\s+const float active_width_m = spray::g_section_manager\.getActiveWidth\(\);",
        "loop_order_read_then_compute_width",
    ),
    (
        "Spray-controller.ino",
        r"uint8_t duty = spray::PWM_MIN;\s+if \(run_enabled\) \{\s+duty = spray::g_flow_controller\.computePumpDuty\(speed_kmh, active_width_m, measured_flow_lpm\);\s+\} else \{\s+spray::g_flow_controller\.stop\(\);\s+\}",
        "hold_forces_safe_pump_state",
    ),
    (
        "Spray-controller.ino",
        r"spray::publishStatus\(measured_flow_lpm, duty, run_enabled, spray::getStatusFaultBitfield\(\)\);",
        "compact_status_publish",
    ),
]

for path, pattern, label in checks:
    require(path, pattern, label)

print("p2_closure_ok")
