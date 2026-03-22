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
        r"`ST:<lpm>,<pump_duty>,<run>,<section_mask>\\n`",
    ),
    ("protocol_version", r"`PROTOCOL_V1`"),
    ("note_lpm", r"`lpm` is the measured total flow"),
    ("note_pump_duty", r"`pump_duty` is the PWM duty in range `0-255`"),
    ("note_run", r"`run` is `1` for RUN and `0` for HOLD"),
    ("note_section_mask", r"`section_mask` packs section states in LSB-first order:"),
    ("note_bit0", r"`bit0=section0`"),
    ("note_bit1", r"`bit1=section1`"),
    ("note_bit2", r"`bit2=section2`"),
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
