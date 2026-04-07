from __future__ import annotations

from gui.bench_app.controller import BenchAppController, ControllerState
from gui.bench_app.wave_d import execute_gui_wave_d


def test_execute_gui_wave_d_deterministic_sequence() -> None:
    controller = BenchAppController()

    result = execute_gui_wave_d(controller)

    assert result.transitions == (
        "start_replay",
        "start_live",
        "stop",
        "start_live",
        "start_replay",
        "stop",
    )
    assert result.stable_after_illegal_live is True
    assert result.unknown_state_rejected is True
    assert result.stable_after_unknown_state is True
    assert result.final_state is ControllerState.IDLE
    assert result.timer_running is False
    assert result.overlay_text == "Idle mode"


def test_wave_d_type_check() -> None:
    try:
        execute_gui_wave_d("not-controller")  # type: ignore[arg-type]
    except TypeError as exc:
        assert str(exc) == "controller must be BenchAppController"
    else:
        raise AssertionError("TypeError expected")
