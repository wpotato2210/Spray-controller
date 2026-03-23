#!/usr/bin/env python3
"""Deterministic protocol structure validator for PROTOCOLS.md."""

from __future__ import annotations

from pathlib import Path
import re
import sys


PROTOCOLS_PATH = Path("PROTOCOLS.md")

REQUIRED_PATTERNS: tuple[tuple[str, str], ...] = (
    ("status_heading", r"(?m)^### STATUS$"),
    ("status_format_header", r"(?m)^- Format:$"),
    (
        "status_frame",
        r"`ST:<lpm>,<pump_duty>,<run>,<section_mask>,<fault_bits>,<fault_text>\\n`",
    ),
    ("protocol_version", r"`PROTOCOL_V1`"),
    ("note_lpm", r"`lpm` is the measured total flow"),
    ("note_pump_duty", r"`pump_duty` is the PWM duty in range `0-255`"),
    ("note_run", r"`run` is `1` for RUN and `0` for HOLD"),
    ("note_section_mask", r"`section_mask` packs section states in LSB-first order:"),
    ("note_bit0", r"`bit0=section0`"),
    ("note_bit1", r"`bit1=section1`"),
    ("note_bit2", r"`bit2=section2`"),
    ("note_fault_bits", r"`fault_bits` is an unsigned bitfield:"),
    ("note_fault_bit0", r"`bit0` = flow stale timeout active\."),
    ("note_fault_bit1", r"`bit1` = wheel pulse timeout active\."),
    ("note_fault_bit2", r"`bit2` = flow sensor configuration invalid\."),
    ("note_fault_bit3", r"`bit3` = wheel sensor configuration invalid\."),
    ("note_fault_text", r"`fault_text` is deterministic status text:"),
    ("note_fault_ok", r"`OK` when `fault_bits == 0`\."),
    ("note_fault_fault", r"`FAULT` when any fault bit is set\."),
    ("section_heading", r"(?m)^### SECTION TELEMETRY \(P5 scalability mapping\)$"),
    ("section_frame", r"`S:<section_id>,<field_id>,<value>\\n`"),
    ("section_output_field", r"`field_id=0` = output state currently driven by firmware\."),
    ("section_switch_field", r"`field_id=1` = operator switch state sampled from the section input\."),
    ("sensor_heading", r"(?m)^### SENSOR TELEMETRY \(P5 scalability mapping\)$"),
    ("sensor_frame", r"`SN:<sensor_id>,<field_id>,<value>\[,<detail>\]\\n`"),
    ("sensor_flow_id", r"`0` = total flow sensor"),
    ("sensor_wheel_id", r"`1` = wheel speed sensor"),
    ("sensor_pressure_id", r"`2` = pressure sensor \(only emitted when pressure feature is enabled\)"),
    ("sensor_primary_field", r"`field_id=0` = primary measured value\."),
    ("sensor_fault_field", r"`field_id=1` = deterministic fault detail bits for that sensor\."),
)


def main() -> int:
    content = PROTOCOLS_PATH.read_text(encoding="utf-8")

    errors: list[str] = []
    for name, pattern in REQUIRED_PATTERNS:
        if re.search(pattern, content) is None:
            errors.append(f"missing:{name}")

    if errors:
        for err in sorted(errors):
            print(err)
        return 1

    print("protocols_ok")
    return 0


if __name__ == "__main__":
    sys.exit(main())
