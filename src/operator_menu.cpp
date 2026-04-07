#include "operator_menu.h"

namespace spray {
namespace {
constexpr uint32_t kMenuUpdateIntervalMs = LOOP_INTERVAL_MS;
}  // namespace

OperatorMenuStateMachine::OperatorMenuStateMachine()
    : state_(OperatorMenuState::kHome),
      last_update_ms_(0U),
      reset_confirmed_(false),
      flow_calibration_requested_(false),
      wheel_calibration_requested_(false) {}

bool OperatorMenuStateMachine::update(uint32_t now_ms, OperatorMenuEvent event) {
  const bool is_event_driven_update = event != OperatorMenuEvent::kNone;
  if (!is_event_driven_update && (now_ms - last_update_ms_) < kMenuUpdateIntervalMs) {
    return false;
  }
  last_update_ms_ = now_ms;
  reset_confirmed_ = false;
  flow_calibration_requested_ = false;
  wheel_calibration_requested_ = false;

  switch (state_) {
    case OperatorMenuState::kHome:
      if (event == OperatorMenuEvent::kNavigate) {
        state_ = OperatorMenuState::kMenu;
      }
      break;
    case OperatorMenuState::kMenu:
      if (event == OperatorMenuEvent::kCancel) {
        state_ = OperatorMenuState::kHome;
      } else if (event == OperatorMenuEvent::kSelect) {
        state_ = OperatorMenuState::kCounters;
      } else if (event == OperatorMenuEvent::kFlowCalibrate) {
        flow_calibration_requested_ = true;
      } else if (event == OperatorMenuEvent::kWheelCalibrate) {
        wheel_calibration_requested_ = true;
      }
      break;
    case OperatorMenuState::kCounters:
      if (event == OperatorMenuEvent::kCancel) {
        state_ = OperatorMenuState::kMenu;
      } else if (event == OperatorMenuEvent::kSelect) {
        state_ = OperatorMenuState::kResetConfirm;
      }
      break;
    case OperatorMenuState::kResetConfirm:
      if (event == OperatorMenuEvent::kCancel) {
        state_ = OperatorMenuState::kCounters;
      } else if (event == OperatorMenuEvent::kConfirm) {
        state_ = OperatorMenuState::kCounters;
        reset_confirmed_ = true;
      }
      break;
  }

  return true;
}

OperatorMenuState OperatorMenuStateMachine::getState() const { return state_; }

bool OperatorMenuStateMachine::consumeResetConfirmed() {
  const bool reset_confirmed = reset_confirmed_;
  reset_confirmed_ = false;
  return reset_confirmed;
}

bool OperatorMenuStateMachine::consumeFlowCalibrationRequested() {
  const bool requested = flow_calibration_requested_;
  flow_calibration_requested_ = false;
  return requested;
}

bool OperatorMenuStateMachine::consumeWheelCalibrationRequested() {
  const bool requested = wheel_calibration_requested_;
  wheel_calibration_requested_ = false;
  return requested;
}

}  // namespace spray
