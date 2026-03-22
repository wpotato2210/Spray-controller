#ifndef SPRAY_CONTROLLER_CONFIG_H
#define SPRAY_CONTROLLER_CONFIG_H

#include <stdint.h>

namespace spray {

constexpr uint8_t SECTION_COUNT = 3U;

constexpr float TARGET_RATE_LPHA = 100.0f;
constexpr float SECTION_WIDTH_M = 0.5f;

constexpr float KP = 2.0f;
constexpr uint8_t PWM_MIN = 0U;
constexpr uint8_t PWM_MAX = 255U;

constexpr float MIN_SPEED_KMH = 0.5f;
constexpr float MAX_FLOW_LPM = 100.0f;

constexpr uint32_t SECTION_TIMEOUT_MS = 500UL;
constexpr uint32_t LOOP_INTERVAL_MS = 50UL;

constexpr float FLOW_PULSES_PER_LITER = 450.0f;
constexpr float WHEEL_CIRCUMFERENCE_M = 2.0f;
constexpr float WHEEL_PULSES_PER_REV = 20.0f;

}  // namespace spray

#endif  // SPRAY_CONTROLLER_CONFIG_H
