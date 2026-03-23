#!/usr/bin/env python3
"""Deterministic validator for P6 operator calibration override."""

from pathlib import Path
import re
import sys


def require(path: Path, pattern: str, label: str) -> None:
    text = path.read_text()
    if not re.search(pattern, text, re.MULTILINE):
        print(f"missing:{label}:{path}", file=sys.stderr)
        sys.exit(1)


def main() -> None:
    require(Path("include/calibration_store.h"), r"setDefaultsOverrideEnabled", "override_setter")
    require(Path("include/calibration_store.h"), r"defaultsOverrideEnabled", "override_getter")
    require(Path("include/calibration_store.h"), r"use_defaults_override", "override_storage_field")
    require(Path("src/calibration_store.cpp"), r"use_defaults_override_ = block\.use_defaults_override != 0U", "override_load")
    require(Path("src/calibration_store.cpp"), r"block\.use_defaults_override = use_defaults_override_ \? 1U : 0U", "override_save")
    require(Path("src/calibration_store.cpp"), r"if \(use_defaults_override_\) \{\s+static const CalibrationProfile kDefaultProfile = defaultCalibrationProfile\(\);\s+return kDefaultProfile;", "override_runtime_defaults")
    require(Path("CALIBRATION.md"), r"persisted `use_defaults_override` selector", "override_doc_flag")
    require(Path("CALIBRATION.md"), r"Set `use_defaults_override = true`", "override_doc_enable")
    require(Path("CALIBRATION.md"), r"Set `use_defaults_override = false`", "override_doc_disable")
    print("p6_calibration_override_ok")


if __name__ == "__main__":
    main()
