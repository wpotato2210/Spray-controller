#!/usr/bin/env python3
"""Deterministic validator for W-03 (HAL + faults + section indexing invariants)."""

from __future__ import annotations

from pathlib import Path
import re
import sys

DISPLAY_H = Path("include/display.h")
DISPLAY_CPP = Path("src/display.cpp")
FAULT_MANAGER_H = Path("include/fault_manager.h")
FAULT_MANAGER_CPP = Path("src/fault_manager.cpp")
SKETCH = Path("Spray-controller.ino")
PINS_NANO_H = Path("include/pins_nano.h")
PINS_UNO_H = Path("include/pins_uno.h")
PINS_MEGA_H = Path("include/pins_mega2560.h")


def read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def require_regex(pattern: str, text: str, error: str, errors: list[str]) -> None:
    if not re.search(pattern, text, re.MULTILINE):
        errors.append(error)


def main() -> int:
    errors: list[str] = []

    display_h = read(DISPLAY_H)
    display_cpp = read(DISPLAY_CPP)
    fault_h = read(FAULT_MANAGER_H)
    fault_cpp = read(FAULT_MANAGER_CPP)
    sketch = read(SKETCH)

    if "class DisplayAdapter" not in display_h:
        errors.append("display_adapter_interface_missing")
    require_regex(r"virtual\s+void\s+begin\(\)\s*=\s*0", display_h, "display_adapter_begin_contract_missing", errors)
    require_regex(
        r"virtual\s+void\s+render\(const\s+DisplayFrame&\s+frame\)\s*=\s*0",
        display_h,
        "display_adapter_render_contract_missing",
        errors,
    )

    for token, error in [
        ("DISPLAY_BACKEND_ST7920_SPI", "display_backend_st7920_macro_missing"),
        ("DISPLAY_BACKEND_NONE", "display_backend_none_macro_missing"),
        ("#error \"Unsupported DISPLAY_BACKEND selection\"", "display_backend_selection_guard_missing"),
        ("class St7920SerialDisplayAdapter final : public DisplayAdapter", "st7920_display_adapter_missing"),
        ("class NullDisplayAdapter final : public DisplayAdapter", "null_display_adapter_missing"),
        ("U8G2_ST7920_128X64_1_SW_SPI", "st7920_page_buffer_backend_missing"),
    ]:
        if token not in display_cpp:
            errors.append(error)

    for token, error in [
        ("class FaultManager", "fault_manager_class_missing"),
        ("void begin();", "fault_manager_begin_api_missing"),
        ("void update(uint8_t observed_fault_bits);", "fault_manager_update_api_missing"),
        ("uint8_t faultBits() const;", "fault_manager_fault_bits_api_missing"),
        ("bool shouldInhibitPump() const;", "fault_manager_pump_inhibit_api_missing"),
    ]:
        if token not in fault_h:
            errors.append(error)

    for token, error in [
        ("kClearStreakThresholdCycles", "fault_manager_clear_streak_policy_missing"),
        ("latched_fault_bits_ |= observed_fault_bits;", "fault_manager_latching_behavior_missing"),
        ("kPumpInhibitFaultMask", "fault_manager_inhibit_mask_missing"),
    ]:
        if token not in fault_cpp:
            errors.append(error)

    if "g_fault_manager.begin();" not in sketch:
        errors.append("fault_manager_setup_integration_missing")
    require_regex(
        r"g_fault_manager\.update\(\s*(?:spray::)?getObservedStatusFaultBitfield\(\)\s*\)",
        sketch,
        "fault_manager_runtime_update_missing",
        errors,
    )
    if "!g_fault_manager.shouldInhibitPump()" not in sketch:
        errors.append("fault_manager_pump_gate_missing")
    if "getStatusFaultBitfield()" not in sketch:
        errors.append("fault_manager_status_publication_missing")

    require_regex(
        r"void\s+writeSections\(\)\s*\{\s*for\s*\(size_t\s+index\s*=\s*0U;\s*index\s*<\s*SECTION_COUNT;",
        sketch,
        "write_sections_direct_index_loop_missing",
        errors,
    )
    require_regex(
        r"uint8_t\s+getSectionBitmask\(\)\s*\{\s*uint8_t\s+mask\s*=\s*0U;\s*for\s*\(size_t\s+index\s*=\s*0U;\s*index\s*<\s*SECTION_COUNT;",
        sketch,
        "get_section_bitmask_direct_index_loop_missing",
        errors,
    )

    for pins_path in (PINS_NANO_H, PINS_UNO_H, PINS_MEGA_H):
        pins_text = read(pins_path)
        if "sectionIdsAreContiguous(kSectionDescriptors)" not in pins_text:
            errors.append(f"section_descriptor_contiguity_assert_missing:{pins_path}")

    if errors:
        for error in sorted(errors):
            print(error)
        return 1

    print("wave_w_03_ok")
    return 0


if __name__ == "__main__":
    sys.exit(main())
