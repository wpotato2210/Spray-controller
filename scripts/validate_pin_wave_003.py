#!/usr/bin/env python3
"""Deterministic validator for PIN-WAVE-003 Mega-profile policy invariants."""

from __future__ import annotations

from pathlib import Path
import re
import sys

PINS_YAML = Path("pins.yaml")
PINS_SELECTOR_H = Path("include/pins.h")
HARDWARE_MD = Path("HARDWARE.md")

PROFILE_NAME = "arduino_avr_mega2560"
MEGA_INTERRUPT_PINS: set[str] = {"2", "3", "18", "19", "20", "21"}
MEGA_PWM_PINS: set[str] = {"2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "44", "45", "46"}
MEGA_DIGITAL_RANGE: set[str] = {str(pin) for pin in range(22, 54)}
MEGA_ANALOG_RANGE: set[str] = {f"A{pin}" for pin in range(8, 16)}

MEGA_KEYS: tuple[str, ...] = (
    "wheel_sensor",
    "flow_sensor",
    "pump_pwm",
    "pressure_sensor",
    "section_1",
    "section_2",
    "section_3",
    "run_hold",
)



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



def extract_key(block: str, key: str) -> str | None:
    match = re.search(rf"^\s+{re.escape(key)}:\s*([^#\n]+)", block, re.MULTILINE)
    if match is None:
        return None
    return canonical_pin(match.group(1).strip())



CANONICAL_POLICY = {
    "arduino_uno": "supported",
    "arduino_nano": "supported",
    "arduino_mega": "supported",
}


def extract_target_policy(content: str) -> dict[str, str]:
    match = re.search(r"target_policy:\s*\n((?:[ \t]+[a-z_]+:\s*(?:supported|unsupported)\s*\n)+)", content)
    if match is None:
        return {}

    policy: dict[str, str] = {}
    for line in match.group(1).splitlines():
        line = line.strip()
        if not line:
            continue
        key, value = [part.strip() for part in line.split(":", 1)]
        policy[key] = value
    return policy



def main() -> int:
    errors: list[str] = []

    pins_yaml = read(PINS_YAML)
    block = profile_block(pins_yaml, PROFILE_NAME)
    if block is None:
        errors.append(f"missing:profile:{PROFILE_NAME}")
    else:
        values: dict[str, str] = {}
        for key in MEGA_KEYS:
            value = extract_key(block, key)
            if value is None:
                errors.append(f"missing:key:{PROFILE_NAME}:{key}")
                continue
            values[key] = value

        if values:
            wheel = values.get("wheel_sensor")
            flow = values.get("flow_sensor")
            pump = values.get("pump_pwm")

            if wheel is not None and wheel not in MEGA_INTERRUPT_PINS:
                errors.append(f"wheel_interrupt_policy_violation:{PROFILE_NAME}:{wheel}")
            if flow is not None and flow not in MEGA_INTERRUPT_PINS:
                errors.append(f"flow_interrupt_policy_violation:{PROFILE_NAME}:{flow}")
            if wheel is not None and flow is not None and wheel == flow:
                errors.append(f"sensor_pin_collision:{PROFILE_NAME}")

            if pump is not None and pump not in MEGA_PWM_PINS:
                errors.append(f"pump_pwm_policy_violation:{PROFILE_NAME}:{pump}")

            bounded_keys = ("section_1", "section_2", "section_3", "run_hold", "pressure_sensor")
            for key in bounded_keys:
                value = values.get(key)
                if value is None:
                    continue
                if value not in MEGA_DIGITAL_RANGE and value not in MEGA_ANALOG_RANGE:
                    errors.append(f"mega_io_range_violation:{PROFILE_NAME}:{key}:{value}")

    pins_selector_policy = extract_target_policy(read(PINS_SELECTOR_H))
    hardware_policy = extract_target_policy(read(HARDWARE_MD))

    if hardware_policy != CANONICAL_POLICY:
        errors.append("hardware_doc_target_policy_mismatch")

    if pins_selector_policy != hardware_policy:
        errors.append("pins_selector_target_policy_drift")

    if errors:
        for error in sorted(errors):
            print(error)
        return 1

    print("pin_wave_003_ok")
    return 0


if __name__ == "__main__":
    sys.exit(main())
