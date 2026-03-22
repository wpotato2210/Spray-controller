#ifndef SPRAY_CONTROLLER_CONFIG_H
#define SPRAY_CONTROLLER_CONFIG_H

#include <stdint.h>

namespace spray {

constexpr uint8_t SECTION_COUNT = 3U;

constexpr float TARGET_RATE_LPHA = 100.0f;
constexpr float SECTION_WIDTH_M = 0.5f;

constexpr float KP = 2.0f;
constexpr float KI = 0.2f;
constexpr float INTEGRAL_MIN = -80.0f;
constexpr float INTEGRAL_MAX = 80.0f;
constexpr float FILTER_ALPHA_FLOW = 0.25f;
constexpr float FILTER_ALPHA_SPEED = 0.25f;
constexpr float FLOW_FAULT_LPM_THRESHOLD = 0.03f;
constexpr uint32_t SENSOR_FAULT_TIMEOUT_MS = 1200UL;
constexpr float MIN_TARGET_FLOW_FOR_FAULT_LPM = 0.2f;
constexpr float MAX_DELTA_PWM_PER_CYCLE = 12.0f;
constexpr uint8_t PWM_MIN = 0U;
constexpr uint8_t PWM_MAX = 255U;
constexpr uint8_t SAFE_FALLBACK_DUTY = PWM_MIN;

constexpr float MIN_SPEED_KMH = 0.5f;
constexpr float MAX_FLOW_LPM = 100.0f;

constexpr uint32_t LOOP_INTERVAL_MS = 50UL;

constexpr float FLOW_PULSES_PER_LITER = 450.0f;
constexpr float WHEEL_CIRCUMFERENCE_M = 2.0f;
constexpr float WHEEL_PULSES_PER_REV = 20.0f;

}  // namespace spray

#endif  // SPRAY_CONTROLLER_CONFIG_H
