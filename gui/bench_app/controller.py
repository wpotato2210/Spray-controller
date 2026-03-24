from __future__ import annotations

from dataclasses import dataclass
from enum import Enum


class ControllerState(str, Enum):
    IDLE = "idle"
    REPLAY = "replay"
    LIVE = "live"


@dataclass
class RuntimeState:
    controller_state: ControllerState = ControllerState.IDLE


class BenchAppController:
    """Controller orchestration for replay/live state transitions."""

    def __init__(self) -> None:
        self.runtime_state = RuntimeState()
        self._cycle_timer_running = False
        self.overlay_text: str | None = None
        self._button_state: ControllerState = ControllerState.IDLE
        self._transition_log: list[str] = []

    def _emit_start_replay(self) -> None:
        self._transition_log.append("start_replay")
        self._on_controller_state_entered(ControllerState.REPLAY)

    def _emit_start_live(self) -> None:
        self._transition_log.append("start_live")
        if self.runtime_state.controller_state is ControllerState.REPLAY:
            raise ValueError("Illegal transition replay->live")
        self._on_controller_state_entered(ControllerState.LIVE)

    def _emit_stop(self) -> None:
        self._transition_log.append("stop")
        self._on_controller_state_entered(ControllerState.IDLE)

    def _transition_to(self, state: ControllerState) -> None:
        transition_map = {
            ControllerState.REPLAY: self._emit_start_replay,
            ControllerState.LIVE: self._emit_start_live,
            ControllerState.IDLE: self._emit_stop,
        }
        transition = transition_map[state]

        try:
            transition()
        except ValueError:
            # Invalid transition: keep state as-is and do not show text implying a change.
            return

    def _on_controller_state_entered(self, state: ControllerState) -> None:
        self.runtime_state.controller_state = state
        self._cycle_timer_running = state in (ControllerState.REPLAY, ControllerState.LIVE)
        self._button_state = state
        self.overlay_text = f"{state.value.title()} mode"
