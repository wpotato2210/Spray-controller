#!/usr/bin/env python3
"""Deterministic validator for W-04 (validation + HIL + CI invariants)."""

from __future__ import annotations

from pathlib import Path
import sys

VALIDATE_SH = Path("scripts/validate.sh")
TESTING_MD = Path("TESTING.md")
VALIDATE_WORKFLOW = Path(".github/workflows/validate.yml")
BUILD_WORKFLOW = Path(".github/workflows/build.yml")
HIL_PROTOCOL = Path("validation/hil_wave_04_protocol.md")


def read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def main() -> int:
    errors: list[str] = []

    validate_sh = read(VALIDATE_SH)
    testing_md = read(TESTING_MD)
    validate_workflow = read(VALIDATE_WORKFLOW)
    build_workflow = read(BUILD_WORKFLOW)

    if "WAVE-04 static analysis check" not in validate_sh:
        errors.append("validate_sh_wave_04_static_gate_missing")
    if "WAVE-04 native unit check" not in validate_sh:
        errors.append("validate_sh_wave_04_unit_gate_missing")

    if "## WAVE-04 HIL Protocol" not in testing_md:
        errors.append("testing_wave_04_hil_section_missing")
    for required in (
        "ISR pulse-rate validation across low/mid/high rates.",
        "Serial saturation with bounded ingress/telemetry behavior.",
        "Run/hold and section-switch bounce rejection with debounce stability.",
    ):
        if required not in testing_md:
            errors.append(f"testing_wave_04_hil_scenario_missing:{required}")

    if not HIL_PROTOCOL.exists():
        errors.append("wave_04_hil_protocol_file_missing")

    if "wave-04-static:" not in validate_workflow:
        errors.append("validate_workflow_wave_04_static_job_missing")
    if "wave-04-unit:" not in validate_workflow:
        errors.append("validate_workflow_wave_04_unit_job_missing")

    for fqbn in ("arduino:avr:nano", "arduino:avr:uno", "arduino:avr:mega"):
        if fqbn not in build_workflow:
            errors.append(f"build_matrix_target_missing:{fqbn}")

    if errors:
        for error in sorted(errors):
            print(error)
        return 1

    print("wave_w_04_ok")
    return 0


if __name__ == "__main__":
    sys.exit(main())
