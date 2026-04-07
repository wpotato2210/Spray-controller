from __future__ import annotations

from dataclasses import dataclass

from .controller import BenchAppController, ControllerState


@dataclass(frozen=True)
class WaveCResult:
    transitions: tuple[str, ...]
    stable_after_illegal_live: bool
    unknown_state_rejected: bool
    final_state: ControllerState
    timer_running: bool
    overlay_text: str | None


def execute_gui_wave_c(controller: BenchAppController) -> WaveCResult:
    """Execute deterministic GUI Wave C mixed transition validation.

    Wave C contract:
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

    unknown_state_rejected = False
    try:
        controller._transition_to("unknown")  # type: ignore[arg-type]
    except ValueError:
        unknown_state_rejected = True

    return WaveCResult(
        transitions=tuple(controller._transition_log[start_index:]),
        stable_after_illegal_live=before_illegal == after_illegal,
        unknown_state_rejected=unknown_state_rejected,
        final_state=controller.runtime_state.controller_state,
        timer_running=controller._cycle_timer_running,
        overlay_text=controller.overlay_text,
    )
