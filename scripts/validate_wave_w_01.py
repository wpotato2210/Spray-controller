#!/usr/bin/env python3
"""Deterministic validator for W-01 (memory + pin-policy + dependency docs)."""

from __future__ import annotations

from pathlib import Path
import re
import sys

DISPLAY_CPP = Path("src/display.cpp")
PIN_POLICY_HEADER = Path("include/pin_policy_guards.h")
README_MD = Path("README.md")
INSTALL_MD = Path("INSTALL.md")


def read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def main() -> int:
    errors: list[str] = []

    display = read(DISPLAY_CPP)
    pin_policy = read(PIN_POLICY_HEADER)
    readme = read(README_MD)
    install = read(INSTALL_MD)

    if re.search(r"U8G2_ST7920_128X64_(?:F|2)_", display):
        errors.append("display_mode_not_page_buffer")

    if not re.search(r"U8G2_ST7920_128X64_1_", display):
        errors.append("display_page_buffer_ctor_missing")

    if not re.search(r"constexpr\s+std::array<uint8_t,\s*16>\s+kAssignedRolePins", pin_policy):
        errors.append("assigned_role_pin_matrix_missing")

    if "containsAnyAssignedPin(detail::kAssignedRolePins, kReservedLcdPins)" not in pin_policy:
        errors.append("assigned_pins_reserved_overlap_assert_missing")

    if "U8g2" not in readme:
        errors.append("readme_u8g2_dependency_missing")

    if "U8g2" not in install:
        errors.append("install_u8g2_dependency_missing")

    if errors:
        for error in sorted(errors):
            print(error)
        return 1

    print("wave_w_01_ok")
    return 0


if __name__ == "__main__":
    sys.exit(main())
