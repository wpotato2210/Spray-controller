#include "interfaces.h"

#include "config.h"

namespace spray {
namespace {
float clampValue(float value, float min_value, float max_value) {
  if (value < min_value) {
    return min_value;
  }
  if (value > max_value) {
    return max_value;
  }
  return value;
}
}  // namespace

FlowController::FlowController()
    : current_duty_(static_cast<float>(PWM_MIN)),
      flow_integral_(0.0f),
      filtered_flow_lpm_(0.0f),
      filtered_speed_kmh_(0.0f),
      no_flow_ms_(0U),
      flow_filter_initialized_(false),
      speed_filter_initialized_(false) {}

uint8_t FlowController::computePumpDuty(float speed_kmh,
                                        float active_width_m,
                                        float measured_flow_lpm) {
  if (!speed_filter_initialized_) {
    filtered_speed_kmh_ = speed_kmh;
    speed_filter_initialized_ = true;
  } else {
    filtered_speed_kmh_ += FILTER_ALPHA_SPEED * (speed_kmh - filtered_speed_kmh_);
  }

  if (!flow_filter_initialized_) {
    filtered_flow_lpm_ = measured_flow_lpm;
    flow_filter_initialized_ = true;
  } else {
    filtered_flow_lpm_ += FILTER_ALPHA_FLOW * (measured_flow_lpm - filtered_flow_lpm_);
  }

  if (filtered_speed_kmh_ < MIN_SPEED_KMH || active_width_m <= 0.0f) {
    stop();
    return static_cast<uint8_t>(current_duty_);
  }

  const float target_flow_lpm =
      (filtered_speed_kmh_ * active_width_m * TARGET_RATE_LPHA) / 600.0f;
  const float flow_error_lpm = target_flow_lpm - filtered_flow_lpm_;
  flow_integral_ += flow_error_lpm;
  flow_integral_ = clampValue(flow_integral_, INTEGRAL_MIN, INTEGRAL_MAX);

  if (target_flow_lpm >= MIN_TARGET_FLOW_FOR_FAULT_LPM &&
      measured_flow_lpm <= FLOW_FAULT_LPM_THRESHOLD) {
    no_flow_ms_ += LOOP_INTERVAL_MS;
  } else {
    no_flow_ms_ = 0U;
  }

  if (no_flow_ms_ >= SENSOR_FAULT_TIMEOUT_MS) {
    current_duty_ = static_cast<float>(SAFE_FALLBACK_DUTY);
    flow_integral_ = 0.0f;
    return static_cast<uint8_t>(current_duty_);
  }

  const float requested_duty = current_duty_ + (KP * flow_error_lpm) + (KI * flow_integral_);
  const float lower_bound = current_duty_ - MAX_DELTA_PWM_PER_CYCLE;
  const float upper_bound = current_duty_ + MAX_DELTA_PWM_PER_CYCLE;
  current_duty_ = clampValue(requested_duty, lower_bound, upper_bound);
  current_duty_ = clampValue(current_duty_, static_cast<float>(PWM_MIN), static_cast<float>(PWM_MAX));

  return static_cast<uint8_t>(current_duty_ + 0.5f);
}

void FlowController::stop() {
  current_duty_ = static_cast<float>(PWM_MIN);
  flow_integral_ = 0.0f;
  no_flow_ms_ = 0U;
}

}  // namespace spray
