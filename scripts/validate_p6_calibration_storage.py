#!/usr/bin/env python3
"""Deterministic validator for P6 calibration persistence."""

from pathlib import Path
import re


def require(path: Path, pattern: str, label: str) -> None:
    text = path.read_text()
    if not re.search(pattern, text, re.MULTILINE | re.DOTALL):
        raise SystemExit(f"missing:{label}")


def main() -> None:
    require(Path("include/calibration_store.h"), r"class CalibrationStore", "calibration_store_class")
    require(Path("src/calibration_store.cpp"), r"CALIBRATION_STORAGE_MAGIC", "magic_marker")
    require(Path("src/calibration_store.cpp"), r"computeChecksum", "checksum_impl")
    require(Path("src/calibration_store.cpp"), r"EEPROM\.(get|put)", "eeprom_io")
    require(Path("Spray-controller.ino"), r"calibrationStore\(\)\.load\(\)", "boot_load")
    require(Path("src/flow_sensor.cpp"), r"activeFlowPulsesPerLiter\(\)", "flow_runtime_calibration")
    require(Path("src/wheel_sensor.cpp"), r"activeWheel(PulsesPerRev|CircumferenceM)\(\)", "wheel_runtime_calibration")
    print("p6_calibration_storage_ok")


if __name__ == "__main__":
    main()
