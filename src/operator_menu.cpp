#include "operator_menu.h"

namespace spray {
namespace {
constexpr uint32_t kMenuUpdateIntervalMs = LOOP_INTERVAL_MS;
}  // namespace

OperatorMenuStateMachine::OperatorMenuStateMachine()
    : state_(OperatorMenuState::kHome), last_update_ms_(0U), reset_confirmed_(false) {}

bool OperatorMenuStateMachine::update(uint32_t now_ms, OperatorMenuEvent event) {
  if ((now_ms - last_update_ms_) < kMenuUpdateIntervalMs) {
    return false;
  }
  last_update_ms_ = now_ms;
  reset_confirmed_ = false;

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

}  // namespace spray
