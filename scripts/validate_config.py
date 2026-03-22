#!/usr/bin/env python3
"""Deterministic config schema validator for config.yaml."""

from __future__ import annotations

from pathlib import Path
import sys


CONFIG_PATH = Path("config.yaml")

SCHEMA: dict[str, object] = {
    "application": {"target_rate_lpha": "number", "section_width_m": "number"},
    "control": {
        "gains": {"kp": "number", "ki": "number"},
        "pwm_min": "int",
        "pwm_max": "int",
        "integral_limits": {"min": "number", "max": "number"},
        "slew_limit_pwm_per_cycle": "number",
    },
    "limits": {
        "min_speed_kmh": "number",
        "max_flow_lpm": "number",
        "flow_fault_lpm_threshold": "number",
        "min_target_flow_for_fault_lpm": "number",
    },
    "timing": {
        "loop_interval_ms": "int",
        "telemetry_interval_ms": "int",
        "sensor_fault_timeout_ms": "int",
    },
}


def _parse_scalar(raw: str) -> object:
    value = raw.strip()
    if value == "":
        return ""
    if value.startswith('"') and value.endswith('"'):
        return value[1:-1]
    if value.startswith("'") and value.endswith("'"):
        return value[1:-1]
    if value.lower() in {"true", "false"}:
        return value.lower() == "true"
    if value.lower() == "null":
        return None
    try:
        if any(ch in value for ch in (".", "e", "E")):
            return float(value)
        return int(value)
    except ValueError:
        return value


def _parse_simple_yaml(text: str) -> dict[str, object]:
    root: dict[str, object] = {}
    stack: list[tuple[int, dict[str, object]]] = [(0, root)]

    for lineno, raw_line in enumerate(text.splitlines(), start=1):
        line = raw_line.rstrip()
        if not line or line.lstrip().startswith("#"):
            continue

        indent = len(line) - len(line.lstrip(" "))
        if indent % 2 != 0:
            raise ValueError(f"invalid-indent:{lineno}")

        stripped = line[indent:]
        if ":" not in stripped:
            raise ValueError(f"invalid-line:{lineno}")

        key, raw_value = stripped.split(":", 1)
        key = key.strip()
        if not key:
            raise ValueError(f"invalid-key:{lineno}")

        while stack and indent < stack[-1][0]:
            stack.pop()
        if not stack:
            raise ValueError(f"invalid-structure:{lineno}")
        if indent > stack[-1][0] and indent != stack[-1][0] + 2:
            raise ValueError(f"invalid-nesting:{lineno}")

        current = stack[-1][1]
        if raw_value.strip() == "":
            nxt: dict[str, object] = {}
            current[key] = nxt
            stack.append((indent + 2, nxt))
        else:
            current[key] = _parse_scalar(raw_value)

    return root


def _validate_mapping(node: object, schema: dict[str, object], prefix: str = "") -> list[str]:
    errors: list[str] = []

    if not isinstance(node, dict):
        return [f"type:{prefix or 'root'}:expected_mapping"]

    keys = set(node.keys())
    schema_keys = set(schema.keys())

    for key in sorted(schema_keys - keys):
        path = f"{prefix}.{key}" if prefix else key
        errors.append(f"missing:{path}")

    for key in sorted(keys - schema_keys):
        path = f"{prefix}.{key}" if prefix else key
        errors.append(f"unknown:{path}")

    for key in sorted(schema_keys & keys):
        path = f"{prefix}.{key}" if prefix else key
        expected = schema[key]
        value = node[key]

        if isinstance(expected, dict):
            errors.extend(_validate_mapping(value, expected, path))
        elif expected == "int" and not isinstance(value, int):
            errors.append(f"type:{path}:expected_int")
        elif expected == "number" and not isinstance(value, (int, float)):
            errors.append(f"type:{path}:expected_number")

    return errors


def main() -> int:
    text = CONFIG_PATH.read_text(encoding="utf-8")
    try:
        data = _parse_simple_yaml(text)
    except ValueError as exc:
        print(str(exc))
        return 1

    errors = sorted(_validate_mapping(data, SCHEMA))
    if errors:
        for err in errors:
            print(err)
        return 1

    print("config_ok")
    return 0


if __name__ == "__main__":
    sys.exit(main())
