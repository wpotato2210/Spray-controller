#!/usr/bin/env python3
"""Deterministic validator for PIN-WAVE-002 pin-map parity invariants."""

from __future__ import annotations

from pathlib import Path
import re
import sys

PINS_YAML = Path("pins.yaml")
PINS_NANO_H = Path("include/pins_nano.h")
PINS_UNO_H = Path("include/pins_uno.h")
PINS_SELECTOR_H = Path("include/pins.h")
HARDWARE_MD = Path("HARDWARE.md")

PROFILE_BY_HEADER: dict[Path, str] = {
    PINS_NANO_H: "arduino_avr_nano",
    PINS_UNO_H: "arduino_avr_uno",
}

YAML_TO_HEADER_KEYS: dict[str, str] = {
    "wheel_sensor": "PIN_WHEEL_SENSOR",
    "flow_sensor": "PIN_FLOW_SENSOR",
    "pump_pwm": "PIN_PUMP_PWM",
    "boom_sections.section_1": "PIN_BOOM_1",
    "boom_sections.section_2": "PIN_BOOM_2",
    "boom_sections.section_3": "PIN_BOOM_3",
    "leds.section_1": "PIN_LED_SECTION_1",
    "switches.run_hold": "PIN_RUN_HOLD",
    "switches.section_switches.section_1": "PIN_SECTION_SW_1",
    "switches.section_switches.section_2": "PIN_SECTION_SW_2",
    "switches.section_switches.section_3": "PIN_SECTION_SW_3",
}


def read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def canonical_pin(value: str) -> str:
    return value.replace(" ", "").upper()


def profile_block(content: str, profile_name: str) -> str | None:
    starts = list(re.finditer(r"^  ([a-z0-9_]+):\s*$", content, re.MULTILINE))
    for index, match in enumerate(starts):
        if match.group(1) != profile_name:
            continue
        start = match.end() + 1
        end = starts[index + 1].start() if index + 1 < len(starts) else len(content)
        return content[start:end]
    return None


YAML_KEY_PATTERNS: dict[str, str] = {
    "wheel_sensor": r"^\s{4}wheel_sensor:\s*([^#\n]+)",
    "flow_sensor": r"^\s{4}flow_sensor:\s*([^#\n]+)",
    "pump_pwm": r"^\s{4}pump_pwm:\s*([^#\n]+)",
    "boom_sections.section_1": r"^\s{4}boom_sections:\s*\n(?:.*\n)*?\s{6}section_1:\s*([^#\n]+)",
    "boom_sections.section_2": r"^\s{4}boom_sections:\s*\n(?:.*\n)*?\s{6}section_2:\s*([^#\n]+)",
    "boom_sections.section_3": r"^\s{4}boom_sections:\s*\n(?:.*\n)*?\s{6}section_3:\s*([^#\n]+)",
    "leds.section_1": r"^\s{4}leds:\s*\n(?:.*\n)*?\s{6}section_1:\s*([^#\n]+)",
    "switches.run_hold": r"^\s{4}switches:\s*\n(?:.*\n)*?\s{6}run_hold:\s*([^#\n]+)",
    "switches.section_switches.section_1": r"^\s{4}switches:\s*\n(?:.*\n)*?\s{6}section_switches:\s*\n(?:.*\n)*?\s{8}section_1:\s*([^#\n]+)",
    "switches.section_switches.section_2": r"^\s{4}switches:\s*\n(?:.*\n)*?\s{6}section_switches:\s*\n(?:.*\n)*?\s{8}section_2:\s*([^#\n]+)",
    "switches.section_switches.section_3": r"^\s{4}switches:\s*\n(?:.*\n)*?\s{6}section_switches:\s*\n(?:.*\n)*?\s{8}section_3:\s*([^#\n]+)",
}


def extract_path_value(profile_block_text: str, key: str) -> str | None:
    pattern = YAML_KEY_PATTERNS[key]
    match = re.search(pattern, profile_block_text, re.MULTILINE)
    if match is None:
        return None
    return canonical_pin(match.group(1).strip())


def extract_header_pin_map(content: str) -> dict[str, str]:
    pairs = re.findall(r"^constexpr\s+uint8_t\s+([A-Z0-9_]+)\s*=\s*([^;]+);", content, re.MULTILINE)
    mapping: dict[str, str] = {}
    for name, value in pairs:
        normalized = value.strip().rstrip("U")
        mapping[name] = canonical_pin(normalized)
    return mapping


def hardware_uses_shared_uno_mapping(content: str) -> bool:
    return "Uses the same core IO mapping as Nano" in content or "Uses the same mapping as Nano" in content


def pins_selector_has_supported_targets(content: str) -> bool:
    has_nano = "#if defined(ARDUINO_AVR_NANO)" in content
    has_uno = "#elif defined(ARDUINO_AVR_UNO)" in content
    has_mega = "#elif defined(ARDUINO_AVR_MEGA2560) || defined(ARDUINO_AVR_MEGA)" in content
    return has_nano and has_uno and has_mega and "Unsupported board target" in content


def main() -> int:
    errors: list[str] = []

    pins_yaml = read(PINS_YAML)
    for header_path, profile in PROFILE_BY_HEADER.items():
        block = profile_block(pins_yaml, profile)
        if block is None:
            errors.append(f"missing:profile:{profile}")
            continue

        header_map = extract_header_pin_map(read(header_path))

        for yaml_key, header_key in YAML_TO_HEADER_KEYS.items():
            yaml_value = extract_path_value(block, yaml_key)
            if yaml_value is None:
                errors.append(f"missing:yaml_key:{profile}:{yaml_key}")
                continue
            header_value = header_map.get(header_key)
            if header_value is None:
                errors.append(f"missing:header_key:{header_path}:{header_key}")
                continue
            if yaml_value != header_value:
                errors.append(
                    f"pin_parity_mismatch:{profile}:{yaml_key}:{yaml_value}!={header_value}"
                )

    hardware_md = read(HARDWARE_MD)
    if not hardware_uses_shared_uno_mapping(hardware_md):
        errors.append("hardware_doc_missing_shared_uno_mapping_note")

    pins_selector = read(PINS_SELECTOR_H)
    if not pins_selector_has_supported_targets(pins_selector):
        errors.append("pins_selector_target_policy_drift")

    if errors:
        for error in sorted(errors):
            print(error)
        return 1

    print("pin_wave_002_ok")
    return 0


if __name__ == "__main__":
    sys.exit(main())
