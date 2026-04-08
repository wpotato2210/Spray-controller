#!/usr/bin/env python3
"""Deterministic validator for PIN-WAVE-001 pin-map invariants."""

from __future__ import annotations

from pathlib import Path
import re
import sys

PINS_YAML = Path("pins.yaml")

PROFILE_NAMES: tuple[str, ...] = (
    "arduino_avr_nano",
    "arduino_avr_uno",
    "arduino_avr_mega2560",
)
INTERRUPT_PINS: dict[str, set[str]] = {
    "arduino_avr_nano": {"2", "3"},
    "arduino_avr_uno": {"2", "3"},
    "arduino_avr_mega2560": {"2", "3", "18", "19", "20", "21"},
}
PWM_PINS: dict[str, set[str]] = {
    "arduino_avr_nano": {"3", "5", "6", "9", "10", "11"},
    "arduino_avr_uno": {"3", "5", "6", "9", "10", "11"},
    "arduino_avr_mega2560": {"2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "44", "45", "46"},
}
REQUIRED_KEYS: tuple[str, ...] = (
    "wheel_sensor",
    "flow_sensor",
    "pump_pwm",
    "section_1",
    "section_2",
    "section_3",
    "run_hold",
)


def read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


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
    return match.group(1).strip()


def canonical_pin(value: str) -> str:
    return value.replace(" ", "").upper()


def main() -> int:
    errors: list[str] = []
    pins_yaml = read(PINS_YAML)

    for profile in PROFILE_NAMES:
        block = profile_block(pins_yaml, profile)
        if block is None:
            errors.append(f"missing:profile:{profile}")
            continue

        values: dict[str, str] = {}
        for key in REQUIRED_KEYS:
            value = extract_key(block, key)
            if value is None:
                errors.append(f"missing:key:{profile}:{key}")
                continue
            values[key] = canonical_pin(value)

        if set(values) != set(REQUIRED_KEYS):
            continue

        ordered_values = [values[key] for key in REQUIRED_KEYS]
        if len(set(ordered_values)) != len(ordered_values):
            errors.append(f"duplicate_pin_assignment:{profile}")

        if values["wheel_sensor"] == values["flow_sensor"]:
            errors.append(f"sensor_pin_collision:{profile}")

        interrupt_policy = INTERRUPT_PINS[profile]
        if values["wheel_sensor"] not in interrupt_policy:
            errors.append(f"wheel_interrupt_policy_violation:{profile}:{values['wheel_sensor']}")
        if values["flow_sensor"] not in interrupt_policy:
            errors.append(f"flow_interrupt_policy_violation:{profile}:{values['flow_sensor']}")

        pwm_policy = PWM_PINS[profile]
        if values["pump_pwm"] not in pwm_policy:
            errors.append(f"pump_pwm_policy_violation:{profile}:{values['pump_pwm']}")

    if errors:
        for error in sorted(errors):
            print(error)
        return 1

    print("pin_wave_001_ok")
    return 0


if __name__ == "__main__":
    sys.exit(main())
