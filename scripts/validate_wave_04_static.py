#!/usr/bin/env python3
"""Deterministic static checks for WAVE-04 quality validation."""

from __future__ import annotations

from pathlib import Path
import re
import subprocess
import sys

SCAN_PATHS = (Path("src"), Path("include"), Path("Spray-controller.ino"))
TODO_PATTERN = re.compile(r"\b(TODO|FIXME|XXX)\b")


def iter_code_files() -> list[Path]:
    files: list[Path] = []
    for path in SCAN_PATHS:
        if path.is_file():
            files.append(path)
            continue
        files.extend(
            sorted(
                p
                for p in path.rglob("*")
                if p.is_file() and p.suffix in {".h", ".hpp", ".c", ".cpp", ".ino"}
            )
        )
    return files


def check_dead_placeholders(files: list[Path]) -> list[str]:
    errors: list[str] = []
    for path in files:
        text = path.read_text(encoding="utf-8")
        for line_no, line in enumerate(text.splitlines(), start=1):
            if TODO_PATTERN.search(line):
                errors.append(f"todo_marker:{path}:{line_no}")
    return errors


def check_include_hygiene(files: list[Path]) -> list[str]:
    errors: list[str] = []
    include_pattern = re.compile(r"^\s*#include\s+([<\"].+[>\"])")

    for path in files:
        seen: set[str] = set()
        for line_no, line in enumerate(path.read_text(encoding="utf-8").splitlines(), start=1):
            match = include_pattern.match(line)
            if match is None:
                continue
            include_token = match.group(1)
            if include_token in seen:
                errors.append(f"duplicate_include:{path}:{line_no}:{include_token}")
            seen.add(include_token)
            if "../" in include_token:
                errors.append(f"parent_include:{path}:{line_no}:{include_token}")
    return errors


def check_protocol_drift() -> list[str]:
    result = subprocess.run(
        [sys.executable, "scripts/validate_protocols.py"],
        capture_output=True,
        text=True,
        check=False,
    )
    if result.returncode == 0 and "protocols_ok" in result.stdout:
        return []
    return [
        "protocol_drift_check_failed",
        result.stdout.strip() or "protocol_validator_stdout_empty",
        result.stderr.strip() or "protocol_validator_stderr_empty",
    ]


def main() -> int:
    files = iter_code_files()
    errors: list[str] = []
    errors.extend(check_dead_placeholders(files))
    errors.extend(check_include_hygiene(files))
    errors.extend(check_protocol_drift())

    if errors:
        for error in errors:
            print(error)
        return 1

    print("wave_04_static_ok")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
