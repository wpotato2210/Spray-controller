#!/usr/bin/env python3
"""Deterministic P4 operator-interface validator."""

from __future__ import annotations

from pathlib import Path
import re
import sys


CHECKS: tuple[tuple[str, Path, str], ...] = (
    (
        "menu_state_enum_contract",
        Path("include/operator_menu.h"),
        r"enum\s+class\s+OperatorMenuState[\s\S]*?kHome\s*=\s*0U,[\s\S]*?kMenu\s*=\s*1U,[\s\S]*?kCounters\s*=\s*2U,[\s\S]*?kResetConfirm\s*=\s*3U",
    ),
    (
        "menu_event_enum_contract",
        Path("include/operator_menu.h"),
        r"enum\s+class\s+OperatorMenuEvent[\s\S]*?kNone\s*=\s*0U,[\s\S]*?kNavigate\s*=\s*1U,[\s\S]*?kSelect\s*=\s*2U,[\s\S]*?kCancel\s*=\s*3U,[\s\S]*?kConfirm\s*=\s*4U",
    ),
    (
        "menu_transition_home_to_menu",
        Path("src/operator_menu.cpp"),
        r"case\s+OperatorMenuState::kHome:[\s\S]*?if\s*\(event\s*==\s*OperatorMenuEvent::kNavigate\)\s*\{[\s\S]*?state_\s*=\s*OperatorMenuState::kMenu;",
    ),
    (
        "menu_transition_menu",
        Path("src/operator_menu.cpp"),
        r"case\s+OperatorMenuState::kMenu:[\s\S]*?if\s*\(event\s*==\s*OperatorMenuEvent::kCancel\)\s*\{[\s\S]*?state_\s*=\s*OperatorMenuState::kHome;[\s\S]*?\}\s*else\s+if\s*\(event\s*==\s*OperatorMenuEvent::kSelect\)\s*\{[\s\S]*?state_\s*=\s*OperatorMenuState::kCounters;",
    ),
    (
        "menu_transition_counters",
        Path("src/operator_menu.cpp"),
        r"case\s+OperatorMenuState::kCounters:[\s\S]*?if\s*\(event\s*==\s*OperatorMenuEvent::kCancel\)\s*\{[\s\S]*?state_\s*=\s*OperatorMenuState::kMenu;[\s\S]*?\}\s*else\s+if\s*\(event\s*==\s*OperatorMenuEvent::kSelect\)\s*\{[\s\S]*?state_\s*=\s*OperatorMenuState::kResetConfirm;",
    ),
    (
        "menu_transition_reset_confirm",
        Path("src/operator_menu.cpp"),
        r"case\s+OperatorMenuState::kResetConfirm:[\s\S]*?if\s*\(event\s*==\s*OperatorMenuEvent::kCancel\)\s*\{[\s\S]*?state_\s*=\s*OperatorMenuState::kCounters;[\s\S]*?\}\s*else\s+if\s*\(event\s*==\s*OperatorMenuEvent::kConfirm\)\s*\{[\s\S]*?state_\s*=\s*OperatorMenuState::kCounters;[\s\S]*?reset_confirmed_\s*=\s*true;",
    ),
    (
        "menu_command_prefix",
        Path("Spray-controller.ino"),
        r"strncmp\(command_buffer,\s*\"ME:\",\s*3\)",
    ),
    (
        "menu_event_token_mapping",
        Path("Spray-controller.ino"),
        r"strcmp\(token,\s*\"NAV\"\)\s*==\s*0[\s\S]*?strcmp\(token,\s*\"SEL\"\)\s*==\s*0[\s\S]*?strcmp\(token,\s*\"CAN\"\)\s*==\s*0[\s\S]*?strcmp\(token,\s*\"CFM\"\)\s*==\s*0",
    ),
    (
        "menu_state_publish_prefix",
        Path("Spray-controller.ino"),
        r"Serial\.print\(MSG_MENU_STATE_PREFIX\);",
    ),
    (
        "preview_cadence_gate",
        Path("Spray-controller.ino"),
        r"bool\s+shouldPublishPreview\(uint32_t\s+now_ms,\s*uint32_t\s+last_preview_ms\)\s*\{[\s\S]*?\(now_ms\s*-\s*last_preview_ms\)\s*>=\s*PREVIEW_INTERVAL_MS;",
    ),
    (
        "preview_loop_publish",
        Path("Spray-controller.ino"),
        r"if\s*\(spray::shouldPublishPreview\(now_ms,\s*last_preview_ms\)\s*&&\s*spray::hasTelemetryTxCapacity\(\)\)\s*\{[\s\S]*?spray::publishPreview\([\s\S]*?\);[\s\S]*?last_preview_ms\s*=\s*now_ms;",
    ),
    (
        "preview_interval_fixed_to_loop",
        Path("config.h"),
        r"constexpr\s+uint32_t\s+PREVIEW_INTERVAL_MS\s*=\s*LOOP_INTERVAL_MS;",
    ),
    (
        "counter_update_uses_active_width",
        Path("Spray-controller.ino"),
        r"const\s+float\s+active_width_m\s*=\s*spray::g_section_manager\.getActiveWidth\(\);[\s\S]*?spray::g_coverage_accumulator\.update\(speed_kmh,\s*active_width_m,\s*spray::LOOP_INTERVAL_MS\);",
    ),
    (
        "reset_requires_confirm_flag",
        Path("Spray-controller.ino"),
        r"if\s*\(!g_operator_menu\.consumeResetConfirmed\(\)\)\s*\{[\s\S]*?return;",
    ),
    (
        "reset_action_counters_and_calibration_hooks",
        Path("Spray-controller.ino"),
        r"g_coverage_accumulator\.reset\(\);[\s\S]*?g_flow_sensor\.reset\(\);[\s\S]*?g_wheel_sensor\.reset\(\);",
    ),
    (
        "reset_event_frame",
        Path("Spray-controller.ino"),
        r"Serial\.print\(MSG_RESET_EVENT_PREFIX\);[\s\S]*?Serial\.print\(\"COUNTERS_CALIBRATION_RESET\"\);",
    ),
)


def main() -> int:
    errors: list[str] = []
    cache: dict[Path, str] = {}

    for name, path, pattern in CHECKS:
        if path not in cache:
            cache[path] = path.read_text(encoding="utf-8")
        if re.search(pattern, cache[path], flags=re.MULTILINE) is None:
            errors.append(f"missing:{name}")

    if errors:
        for err in sorted(errors):
            print(err)
        return 1

    print("p4_operator_interface_ok")
    return 0


if __name__ == "__main__":
    sys.exit(main())
