from __future__ import annotations

from dataclasses import dataclass

from .controller import BenchAppController, ControllerState


@dataclass(frozen=True)
class WaveDResult:
    transitions: tuple[str, ...]
    stable_after_illegal_live: bool
    unknown_state_rejected: bool
    stable_after_unknown_state: bool
    final_state: ControllerState
    timer_running: bool
    overlay_text: str | None


def execute_gui_wave_d(controller: BenchAppController) -> WaveDResult:
    """Execute deterministic GUI Wave D resilience validation.

    Wave D contract:
    1) REPLAY enter from IDLE.
    2) Attempt illegal LIVE transition from REPLAY (must not commit).
    3) STOP to IDLE.
    4) LIVE enter from IDLE.
    5) REPLAY enter from LIVE.
    6) STOP to IDLE.
    7) Attempt unknown state transition (must raise and keep state stable).
    """
    if not isinstance(controller, BenchAppController):
        raise TypeError("controller must be BenchAppController")

    start_index = len(controller._transition_log)

    controller._transition_to(ControllerState.REPLAY)
    before_illegal = (
        controller.runtime_state.controller_state,
        controller._cycle_timer_running,
        controller.overlay_text,
    )
    controller._transition_to(ControllerState.LIVE)
    after_illegal = (
        controller.runtime_state.controller_state,
        controller._cycle_timer_running,
        controller.overlay_text,
    )

    controller._transition_to(ControllerState.IDLE)
    controller._transition_to(ControllerState.LIVE)
    controller._transition_to(ControllerState.REPLAY)
    controller._transition_to(ControllerState.IDLE)

    before_unknown = (
        controller.runtime_state.controller_state,
        controller._cycle_timer_running,
        controller.overlay_text,
    )
    unknown_state_rejected = False
    try:
        controller._transition_to("unknown")  # type: ignore[arg-type]
    except ValueError:
        unknown_state_rejected = True
    after_unknown = (
        controller.runtime_state.controller_state,
        controller._cycle_timer_running,
        controller.overlay_text,
    )

    return WaveDResult(
        transitions=tuple(controller._transition_log[start_index:]),
        stable_after_illegal_live=before_illegal == after_illegal,
        unknown_state_rejected=unknown_state_rejected,
        stable_after_unknown_state=before_unknown == after_unknown,
        final_state=controller.runtime_state.controller_state,
        timer_running=controller._cycle_timer_running,
        overlay_text=controller.overlay_text,
    )
