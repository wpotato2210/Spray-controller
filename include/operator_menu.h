#ifndef SPRAY_CONTROLLER_OPERATOR_MENU_H
#define SPRAY_CONTROLLER_OPERATOR_MENU_H

#include <stdint.h>

#include "config.h"

namespace spray {

enum class OperatorMenuState : uint8_t {
  kHome = 0U,
  kMenu = 1U,
  kCounters = 2U,
  kResetConfirm = 3U
};

enum class OperatorMenuEvent : uint8_t {
  kNone = 0U,
  kNavigate = 1U,
  kSelect = 2U,
  kCancel = 3U,
  kConfirm = 4U,
  kFlowCalibrate = 5U,
  kWheelCalibrate = 6U
};

class OperatorMenuStateMachine {
 public:
  OperatorMenuStateMachine();
  bool update(uint32_t now_ms, OperatorMenuEvent event);
  OperatorMenuState getState() const;
  bool consumeResetConfirmed();
  bool consumeFlowCalibrationRequested();
  bool consumeWheelCalibrationRequested();

 private:
  OperatorMenuState state_;
  uint32_t last_update_ms_;
  bool reset_confirmed_;
  bool flow_calibration_requested_;
  bool wheel_calibration_requested_;
};

}  // namespace spray

#endif  // SPRAY_CONTROLLER_OPERATOR_MENU_H
