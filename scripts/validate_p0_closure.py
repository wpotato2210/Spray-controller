#!/usr/bin/env python3
"""Deterministic validator for P0 closure artifacts."""

from __future__ import annotations

from pathlib import Path
import re
import sys


CHECKS: tuple[tuple[str, Path, str], ...] = (
    (
        "roadmap_entrypoint_path",
        Path("DEVELOPMENT_ROADMAP.md"),
        r"Current firmware entrypoint in `Spray-controller\.ino`\.",
    ),
    (
        "roadmap_interface_path",
        Path("DEVELOPMENT_ROADMAP.md"),
        r"Public contracts in `include/interfaces\.h`\.",
    ),
    (
        "dev_notes_cadence_source",
        Path("DEV_NOTES.md"),
        r"Canonical cadence source: `LOOP_INTERVAL_MS` from `config\.h`",
    ),
    (
        "dev_notes_baseline_sections",
        Path("DEV_NOTES.md"),
        r"Three binary boom sections: topology is fixed by `SECTION_COUNT == 3`",
    ),
    (
        "audit_roadmap_alignment_note",
        Path("PHASE_ZERO_READINESS_AUDIT.md"),
        r"roadmap P0 inputs and tasks are aligned to the actual sketch entrypoint\s+`Spray-controller\.ino`",
    ),
    (
        "audit_go_decision",
        Path("PHASE_ZERO_READINESS_AUDIT.md"),
        r"\*\*GO for P0 closure\*\*",
    ),
    (
        "changelog_p0_closure_record",
        Path("CHANGELOG.md"),
        r"Executed `P0-CLS-003` by extending\s+`scripts/validate_p0_closure\.py`\s+to enforce the required `CHANGELOG\.md`\s+closure record for P0",
    ),
    (
        "interface_checklist_flow_controller_owner",
        Path("include_path_audit.md"),
        r"\|`FlowController`\|`include/interfaces\.h`\|`src/flow_controller\.cpp`\|",
    ),
    (
        "interface_checklist_coverage_owner",
        Path("include_path_audit.md"),
        r"\|`CoverageAccumulator`\|`include/interfaces\.h`\|`src/coverage_accumulator\.cpp`\|",
    ),
    (
        "config_loop_interval_yaml",
        Path("config.yaml"),
        r"loop_interval_ms:\s*50",
    ),
    (
        "config_loop_interval_header",
        Path("config.h"),
        r"constexpr\s+uint32_t\s+LOOP_INTERVAL_MS\s*=\s*50UL;",
    ),
    (
        "loop_guard_uses_header_constant",
        Path("Spray-controller.ino"),
        r"if\s*\(\(now_ms\s*-\s*last_loop_ms\)\s*<\s*spray::LOOP_INTERVAL_MS\)",
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

    print("p0_closure_ok")
    return 0


if __name__ == "__main__":
    sys.exit(main())
