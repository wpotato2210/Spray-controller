from __future__ import annotations

from dataclasses import dataclass

from .controller import BenchAppController, ControllerState


@dataclass(frozen=True)
class WaveAResult:
    transitions: tuple[str, ...]
    final_state: ControllerState
    timer_running: bool
    overlay_text: str | None


def execute_gui_wave_a(controller: BenchAppController) -> WaveAResult:
    """Execute deterministic GUI Wave A transition sweep.

    Wave A contract:
    1) replay entry from idle
    2) stop back to idle
    3) live entry from idle
    4) stop back to idle
    """
    if not isinstance(controller, BenchAppController):
        raise TypeError("controller must be BenchAppController")

    start_index = len(controller._transition_log)
    controller._transition_to(ControllerState.REPLAY)
    controller._transition_to(ControllerState.IDLE)
    controller._transition_to(ControllerState.LIVE)
    controller._transition_to(ControllerState.IDLE)

    transitions = tuple(controller._transition_log[start_index:])
    return WaveAResult(
        transitions=transitions,
        final_state=controller.runtime_state.controller_state,
        timer_running=controller._cycle_timer_running,
        overlay_text=controller.overlay_text,
    )
