#!/usr/bin/env python3
"""Deterministic validator for P5 scalability invariants."""

from __future__ import annotations

from pathlib import Path
import re
import sys

CONFIG_PATH = Path("config.h")
NANO_PINS_PATH = Path("include/pins_nano.h")
UNO_PINS_PATH = Path("include/pins_uno.h")
PROTOCOL_PATH = Path("include/protocol.h")
SKETCH_PATH = Path("Spray-controller.ino")


def read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def require_match(content: str, pattern: str, error: str, errors: list[str]) -> re.Match[str] | None:
    match = re.search(pattern, content, re.MULTILINE | re.DOTALL)
    if match is None:
        errors.append(error)
    return match


def count_descriptor_entries(content: str) -> int:
    block = require_match(
        content,
        r"kSectionDescriptors\s*=\s*\{\{\{(?P<body>.*?)\}\}\};",
        "missing:section_descriptor_block",
        [],
    )
    if block is None:
        return -1
    return len(re.findall(r"\{\d+U,\s*[^,]+,\s*[^}]+\}", block.group("body")))



def main() -> int:
    errors: list[str] = []

    config = read(CONFIG_PATH)
    nano_pins = read(NANO_PINS_PATH)
    uno_pins = read(UNO_PINS_PATH)
    protocol = read(PROTOCOL_PATH)
    sketch = read(SKETCH_PATH)

    section_count_match = require_match(
        config,
        r"constexpr\s+uint8_t\s+SECTION_COUNT\s*=\s*(\d+)U;",
        "missing:section_count",
        errors,
    )
    sensor_count_match = require_match(
        config,
        r"constexpr\s+uint8_t\s+SENSOR_COUNT\s*=\s*\n\s*static_cast<uint8_t>\((\d+)U\s*\+\s*\(ENABLE_PRESSURE_SENSOR\s*\?\s*(\d+)U\s*:\s*(\d+)U\)\);",
        "missing:sensor_count_formula",
        errors,
    )

    if section_count_match is not None:
        section_count = int(section_count_match.group(1))
        for name, pins_content in (("nano", nano_pins), ("uno", uno_pins)):
            descriptor_count = count_descriptor_entries(pins_content)
            if descriptor_count != section_count:
                errors.append(f"section_descriptor_count_mismatch:{name}:{descriptor_count}")

            descriptor_ids = [
                int(value)
                for value in re.findall(r"\{(\d+)U,\s*[^,]+,\s*[^}]+\}", pins_content)
            ]
            expected_ids = list(range(section_count))
            if descriptor_ids != expected_ids:
                errors.append(f"section_descriptor_ids_unstable:{name}:{descriptor_ids}")

        if not re.search(
            r"static_assert\(kSectionDescriptors\.size\(\) == SECTION_COUNT,",
            nano_pins,
        ) or not re.search(
            r"static_assert\(kSectionDescriptors\.size\(\) == SECTION_COUNT,",
            uno_pins,
        ):
            errors.append("missing:descriptor_count_static_assert")

        if not re.search(
            r"uint8_t\s+getSectionBitmask\(\)\s*\{.*?for\s*\(const SectionDescriptor& section : kSectionDescriptors\)\s*\{.*?mask \|= static_cast<uint8_t>\(1U << section\.id\);",
            sketch,
            re.DOTALL,
        ):
            errors.append("missing:section_bitmask_descriptor_loop")

        if section_count == 3:
            for bit in range(section_count):
                if re.search(
                    rf"`bit{bit}=section{bit}`",
                    read(Path("PROTOCOLS.md")),
                ) is None:
                    errors.append(f"missing:status_bit_mapping_bit{bit}")
    if sensor_count_match is not None:
        expected_sensor_count = int(sensor_count_match.group(1)) + int(sensor_count_match.group(3))
        sensor_contract_ids = [
            int(value)
            for value in re.findall(
                r"\{TELEMETRY_SENSOR_ID_[A-Z_]+\}|TELEMETRY_SENSOR_ID_[A-Z_]+ = (\d+)U",
                protocol,
            )
            if value
        ]
        explicit_sensor_ids = [
            int(value)
            for value in re.findall(r"TELEMETRY_SENSOR_ID_[A-Z_]+\s*=\s*(\d+)U", protocol)
        ]
        if explicit_sensor_ids[:expected_sensor_count] != list(range(expected_sensor_count)):
            errors.append(f"sensor_id_mapping_unstable:{explicit_sensor_ids[:expected_sensor_count]}")

    required_patterns = (
        (r"kSectionTelemetryFrameContract\[0\]\.field_id == TELEMETRY_SECTION_FIELD_OUTPUT_STATE", "missing:section_contract_output_order"),
        (r"kSectionTelemetryFrameContract\[1\]\.field_id == TELEMETRY_SECTION_FIELD_SWITCH_STATE", "missing:section_contract_switch_order"),
        (r"kTelemetrySensorContracts\[0\]\.sensor_id == TELEMETRY_SENSOR_ID_FLOW", "missing:sensor_contract_flow_order"),
        (r"kTelemetrySensorContracts\[1\]\.sensor_id == TELEMETRY_SENSOR_ID_WHEEL", "missing:sensor_contract_wheel_order"),
        (r"kTelemetrySensorContracts\[2\]\.sensor_id == TELEMETRY_SENSOR_ID_PRESSURE", "missing:sensor_contract_pressure_order"),
        (r"kSensorTelemetryFrameContract\[0\]\.field_id == TELEMETRY_SENSOR_FIELD_PRIMARY_VALUE", "missing:sensor_field_primary_order"),
        (r"kSensorTelemetryFrameContract\[1\]\.field_id == TELEMETRY_SENSOR_FIELD_FAULT_BITS", "missing:sensor_field_fault_order"),
        (r"for \(const SectionDescriptor& section : kSectionDescriptors\)", "missing:section_descriptor_iteration"),
        (r"for \(const TelemetrySensorContract& sensor : kTelemetrySensorContracts\)", "missing:sensor_contract_iteration"),
    )
    for pattern, error in required_patterns:
        if re.search(pattern, protocol + "\n" + sketch) is None:
            errors.append(error)

    if errors:
        for error in sorted(errors):
            print(error)
        return 1

    print("p5_scalability_ok")
    return 0


if __name__ == "__main__":
    sys.exit(main())
