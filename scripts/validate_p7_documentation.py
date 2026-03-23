#!/usr/bin/env python3
"""Deterministic validator for P7 documentation alignment."""

from pathlib import Path
import re
import sys


def require(text: str, needle: str, label: str) -> None:
    if needle not in text:
        print(f"missing:{label}", file=sys.stderr)
        sys.exit(1)


def main() -> None:
    protocols = Path("PROTOCOLS.md").read_text()
    usage = Path("USAGE.md").read_text()
    testing = Path("TESTING.md").read_text()
    validate_sh = Path("scripts/validate.sh").read_text()

    require(protocols, "ST:<lpm>,<pump_duty>,<run>,<section_mask>,<fault_bits>,<fault_text>", "status_contract")
    require(protocols, "Publish at the fixed main-loop cadence owned by `PREVIEW_INTERVAL_MS`.", "preview_cadence")
    require(protocols, "Accept only a `CFM` event to execute reset.", "reset_confirm_token")
    require(protocols, "Successful reset also restores the persisted calibration profile to defaults", "reset_default_restore")
    require(protocols, "Documentation alignment date for this protocol contract: `2026-03-23`", "protocol_alignment_date")
    require(usage, "TARGET_RATE_LPHA = 100.0", "target_rate")
    require(usage, "SECTION_WIDTH_M = 0.5", "section_width")
    require(usage, "KP = 2.0` and `KI = 0.2", "pi_constants")
    require(usage, "default publish interval is `50 ms`", "preview_interval")
    require(testing, "python3 scripts/validate_p7_documentation.py", "testing_validator_doc")
    require(testing, "pin `11`", "pump_pin")
    require(testing, "pins `12`, `13`, and `A1`", "section_pins")
    require(validate_sh, 'run_step "P7 documentation alignment check"', "validate_hook")

    for text, label in ((protocols, "PROTOCOLS"), (usage, "USAGE"), (testing, "TESTING")):
        if re.search(r"(?m)^## Placeholders$", text):
            print(f"placeholder_section_present:{label}", file=sys.stderr)
            sys.exit(1)

    print("p7_documentation_ok")


if __name__ == "__main__":
    main()
