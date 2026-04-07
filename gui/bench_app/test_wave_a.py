from __future__ import annotations

from gui.bench_app.controller import BenchAppController, ControllerState
from gui.bench_app.wave_a import execute_gui_wave_a


def test_execute_gui_wave_a_deterministic_sequence() -> None:
    controller = BenchAppController()

    result = execute_gui_wave_a(controller)

    assert result.transitions == ("start_replay", "stop", "start_live", "stop")
    assert result.final_state is ControllerState.IDLE
    assert result.timer_running is False
    assert result.overlay_text == "Idle mode"


def test_illegal_replay_to_live_is_ignored() -> None:
    controller = BenchAppController()

    controller._transition_to(ControllerState.REPLAY)
    before = (
        controller.runtime_state.controller_state,
        controller._cycle_timer_running,
        controller.overlay_text,
        len(controller._transition_log),
    )
    controller._transition_to(ControllerState.LIVE)
    after = (
        controller.runtime_state.controller_state,
        controller._cycle_timer_running,
        controller.overlay_text,
        len(controller._transition_log),
    )

    assert before[:3] == after[:3]
    assert after[3] == before[3] + 1
