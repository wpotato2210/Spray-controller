import unittest

from gui.bench_app.controller import BenchAppController, ControllerState


class BenchAppControllerTests(unittest.TestCase):
    def test_transition_relies_on_entered_callback_to_commit_runtime_state(self) -> None:
        controller = BenchAppController()

        observed_runtime_state_before_commit: list[ControllerState] = []

        def tracking_entered(state: ControllerState) -> None:
            observed_runtime_state_before_commit.append(controller.runtime_state.controller_state)
            controller.runtime_state.controller_state = state
            controller._cycle_timer_running = state in (ControllerState.REPLAY, ControllerState.LIVE)
            controller._button_state = state
            controller.overlay_text = f"{state.value.title()} mode"

        controller._on_controller_state_entered = tracking_entered  # type: ignore[method-assign]

        controller._transition_to(ControllerState.LIVE)

        self.assertEqual(observed_runtime_state_before_commit, [ControllerState.IDLE])
        self.assertEqual(controller.runtime_state.controller_state, ControllerState.LIVE)
        self.assertTrue(controller._cycle_timer_running)
        self.assertEqual(controller._button_state, ControllerState.LIVE)
        self.assertEqual(controller.overlay_text, "Live mode")
        self.assertEqual(controller._transition_log, ["start_live"])

    def test_replay_to_live_illegal_transition_keeps_runtime_ui_and_timer(self) -> None:
        controller = BenchAppController()

        entered_states: list[ControllerState] = []
        original_entered = controller._on_controller_state_entered

        def tracking_entered(state: ControllerState) -> None:
            entered_states.append(state)
            original_entered(state)

        controller._on_controller_state_entered = tracking_entered  # type: ignore[method-assign]

        controller._transition_to(ControllerState.REPLAY)
        self.assertEqual(controller.runtime_state.controller_state, ControllerState.REPLAY)
        self.assertTrue(controller._cycle_timer_running)
        self.assertEqual(controller._button_state, ControllerState.REPLAY)
        self.assertEqual(controller.overlay_text, "Replay mode")

        controller._transition_to(ControllerState.LIVE)

        # Invalid transition replay->live should keep the prior stable state.
        self.assertEqual(entered_states, [ControllerState.REPLAY])
        self.assertEqual(controller.runtime_state.controller_state, ControllerState.REPLAY)
        self.assertTrue(controller._cycle_timer_running)
        self.assertEqual(controller._button_state, ControllerState.REPLAY)
        self.assertEqual(controller.overlay_text, "Replay mode")
        self.assertEqual(controller._transition_log, ["start_replay", "start_live"])

    def test_invalid_transition_does_not_mutate_runtime_state_before_completion(self) -> None:
        controller = BenchAppController()
        controller._transition_to(ControllerState.REPLAY)

        entered_states: list[ControllerState] = []
        observed_state_during_transition: list[ControllerState] = []
        observed_timer_during_transition: list[bool] = []
        observed_button_during_transition: list[ControllerState] = []

        def fail_live_transition() -> None:
            controller._transition_log.append("start_live")
            observed_state_during_transition.append(controller.runtime_state.controller_state)
            observed_timer_during_transition.append(controller._cycle_timer_running)
            observed_button_during_transition.append(controller._button_state)
            raise ValueError("Illegal transition replay->live")

        original_entered = controller._on_controller_state_entered

        def tracking_entered(state: ControllerState) -> None:
            entered_states.append(state)
            original_entered(state)

        controller._emit_start_live = fail_live_transition  # type: ignore[method-assign]
        controller._on_controller_state_entered = tracking_entered  # type: ignore[method-assign]

        controller._transition_to(ControllerState.LIVE)

        self.assertEqual(entered_states, [])
        self.assertEqual(observed_state_during_transition, [ControllerState.REPLAY])
        self.assertEqual(observed_timer_during_transition, [True])
        self.assertEqual(observed_button_during_transition, [ControllerState.REPLAY])
        self.assertEqual(controller.runtime_state.controller_state, ControllerState.REPLAY)
        self.assertTrue(controller._cycle_timer_running)
        self.assertEqual(controller._button_state, ControllerState.REPLAY)
        self.assertEqual(controller.overlay_text, "Replay mode")
        self.assertEqual(controller._transition_log, ["start_replay", "start_live"])

    def test_invalid_transition_does_not_emit_overlay_for_target_state(self) -> None:
        controller = BenchAppController()
        controller._transition_to(ControllerState.REPLAY)
        controller.overlay_text = "Replay stable"

        controller._transition_to(ControllerState.LIVE)

        self.assertEqual(controller.runtime_state.controller_state, ControllerState.REPLAY)
        self.assertTrue(controller._cycle_timer_running)
        self.assertEqual(controller._button_state, ControllerState.REPLAY)
        self.assertEqual(controller.overlay_text, "Replay stable")
        self.assertNotEqual(controller.overlay_text, "Live mode")

if __name__ == "__main__":
    unittest.main()
