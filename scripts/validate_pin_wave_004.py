#!/usr/bin/env python3
"""Deterministic validator for PIN-WAVE-004 hardware documentation parity."""

from __future__ import annotations

from pathlib import Path
import re
import sys

MEGA_HEADER = Path("include/pins_mega2560.h")
LCD_HEADER = Path("include/pin_map_lcd.h")
HARDWARE_MD = Path("HARDWARE.md")

MEGA_ROWS: dict[str, str] = {
    "PIN_WHEEL_SENSOR": "Wheel Sensor",
    "PIN_FLOW_SENSOR": "Flow Sensor",
    "PIN_PUMP_PWM": "Pump PWM",
    "PIN_SECTION_RELAY_1": "Boom Section 1",
    "PIN_SECTION_RELAY_2": "Boom Section 2",
    "PIN_SECTION_RELAY_3": "Boom Section 3",
    "PIN_RUN_HOLD": "Run/Hold Switch",
    "PIN_SECTION_SW_1": "Section Switch 1",
    "PIN_SECTION_SW_2": "Section Switch 2",
    "PIN_SECTION_SW_3": "Section Switch 3",
    "PIN_ENCODER_CLK": "Encoder CLK (A)",
    "PIN_ENCODER_DT": "Encoder DT (B)",
    "PIN_ENCODER_SW": "Encoder SW",
    "PIN_BTN_UP": "Button UP",
    "PIN_BTN_DOWN": "Button DOWN",
    "PIN_BTN_CAL": "Button CAL",
    "PIN_BTN_SELECT": "Button SELECT",
    "PIN_BTN_AUTO_MANUAL": "Button AUTO/MANUAL",
}

LCD_ROWS: dict[str, str] = {
    "LCD_CLK": "LCD SCLK / E",
    "LCD_MOSI": "LCD MOSI / RW",
    "LCD_CS": "LCD CS / RS",
    "LCD_RESET": "LCD RST",
}


def read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def extract_uint_defines(content: str, names: set[str]) -> dict[str, str]:
    found: dict[str, str] = {}
    for name, value in re.findall(r"constexpr\s+uint8_t\s+([A-Z0-9_]+)\s*=\s*([^;]+);", content):
        if name in names:
            found[name] = value.strip().rstrip("U")
    return found


def extract_mega_lcd_defines(content: str, names: set[str]) -> dict[str, str]:
    mega_block = re.search(
        r"#if\s+defined\(ARDUINO_AVR_MEGA2560\)\s*\|\|\s*defined\(ARDUINO_AVR_MEGA\)\s*(.*?)#elif",
        content,
        re.DOTALL,
    )
    if mega_block is None:
        return {}
    found: dict[str, str] = {}
    for name, value in re.findall(r"#define\s+([A-Z0-9_]+)\s+([A-Za-z0-9_]+)", mega_block.group(1)):
        if name in names:
            found[name] = value.strip()
    return found


def has_hardware_row(content: str, pin_value: str, label: str) -> bool:
    row_pattern = rf"^\|\s*{re.escape(pin_value)}\s*\|\s*{re.escape(label)}\s*\|"
    return re.search(row_pattern, content, re.MULTILINE) is not None


def main() -> int:
    errors: list[str] = []

    mega_map = extract_uint_defines(read(MEGA_HEADER), set(MEGA_ROWS))
    lcd_map = extract_mega_lcd_defines(read(LCD_HEADER), set(LCD_ROWS))
    hardware = read(HARDWARE_MD)

    for name, label in MEGA_ROWS.items():
        pin_value = mega_map.get(name)
        if pin_value is None:
            errors.append(f"missing:mega_header:{name}")
            continue
        if not has_hardware_row(hardware, pin_value, label):
            errors.append(f"hardware_row_mismatch:{name}:{pin_value}:{label}")

    for name, label in LCD_ROWS.items():
        pin_value = lcd_map.get(name)
        if pin_value is None:
            errors.append(f"missing:lcd_header:{name}")
            continue
        if not has_hardware_row(hardware, pin_value, label):
            errors.append(f"hardware_row_mismatch:{name}:{pin_value}:{label}")

    if mega_map.get("PIN_BTN_SELECT") == lcd_map.get("LCD_RESET"):
        errors.append("mega_select_lcd_reset_conflict")

    if errors:
        for error in sorted(errors):
            print(error)
        return 1

    print("pin_wave_004_ok")
    return 0


if __name__ == "__main__":
    sys.exit(main())
