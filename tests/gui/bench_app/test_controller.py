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


if __name__ == "__main__":
    unittest.main()
