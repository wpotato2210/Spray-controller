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
        """Request a transition via state-machine triggers.

        Runtime/UI state is committed only by `_on_controller_state_entered` after
        a transition completes successfully. This method intentionally does not
        pre-assign `runtime_state.controller_state` before the trigger executes,
        while still emitting the state-machine trigger signal.
        """
        transition_map = {
            ControllerState.REPLAY: self._emit_start_replay,
            ControllerState.LIVE: self._emit_start_live,
            ControllerState.IDLE: self._emit_stop,
        }
        transition = transition_map[state]
        stable_runtime_state = self.runtime_state.controller_state
        stable_timer_running = self._cycle_timer_running
        stable_button_state = self._button_state
        stable_overlay_text = self.overlay_text

        try:
            transition()
        except ValueError:
            # Invalid transition: state remains unchanged because only
            # `_on_controller_state_entered` commits runtime/UI/timer values.
            self.runtime_state.controller_state = stable_runtime_state
            self._cycle_timer_running = stable_timer_running
            self._button_state = stable_button_state
            self.overlay_text = stable_overlay_text
            return

    def _on_controller_state_entered(self, state: ControllerState) -> None:
        self.runtime_state.controller_state = state
        self._cycle_timer_running = state in (ControllerState.REPLAY, ControllerState.LIVE)
        self._button_state = state
        self.overlay_text = f"{state.value.title()} mode"
