#include "include/interfaces.h"

#include "config.h"

namespace spray {

FlowController::FlowController() : current_duty_(static_cast<float>(PWM_MIN)) {}

uint8_t FlowController::computePumpDuty(float speed_kmh,
                                        float active_width_m,
                                        float measured_flow_lpm) {
  if (speed_kmh < MIN_SPEED_KMH || active_width_m <= 0.0f) {
    stop();
    return static_cast<uint8_t>(current_duty_);
  }

  const float target_flow_lpm =
      (speed_kmh * active_width_m * TARGET_RATE_LPHA) / 600.0f;
  const float flow_error_lpm = target_flow_lpm - measured_flow_lpm;
  current_duty_ += KP * flow_error_lpm;

  if (current_duty_ < static_cast<float>(PWM_MIN)) {
    current_duty_ = static_cast<float>(PWM_MIN);
  }
  if (current_duty_ > static_cast<float>(PWM_MAX)) {
    current_duty_ = static_cast<float>(PWM_MAX);
  }

  return static_cast<uint8_t>(current_duty_ + 0.5f);
}

void FlowController::stop() { current_duty_ = static_cast<float>(PWM_MIN); }

}  // namespace spray
