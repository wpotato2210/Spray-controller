import unittest

from gui.bench_app.controller import BenchAppController, ControllerState


class BenchAppControllerTests(unittest.TestCase):
    def test_replay_to_live_illegal_transition_keeps_runtime_ui_and_timer(self) -> None:
        controller = BenchAppController()

        controller._transition_to(ControllerState.REPLAY)
        self.assertEqual(controller.runtime_state.controller_state, ControllerState.REPLAY)
        self.assertTrue(controller._cycle_timer_running)
        self.assertEqual(controller._button_state, ControllerState.REPLAY)
        self.assertEqual(controller.overlay_text, "Replay mode")

        controller._transition_to(ControllerState.LIVE)

        # Invalid transition replay->live should keep the prior stable state.
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

        def fail_live_transition() -> None:
            controller._transition_log.append("start_live")
            observed_state_during_transition.append(controller.runtime_state.controller_state)
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
        self.assertEqual(controller.runtime_state.controller_state, ControllerState.REPLAY)
        self.assertTrue(controller._cycle_timer_running)
        self.assertEqual(controller._button_state, ControllerState.REPLAY)
        self.assertEqual(controller.overlay_text, "Replay mode")
        self.assertEqual(controller._transition_log, ["start_replay", "start_live"])


if __name__ == "__main__":
    unittest.main()
