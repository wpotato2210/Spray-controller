#ifndef SPRAY_CONTROLLER_CONFIG_H
#define SPRAY_CONTROLLER_CONFIG_H

#include <array>
#include <stddef.h>
#include <stdint.h>

namespace spray {

constexpr size_t MAX_SECTIONS = 8U;
constexpr size_t MAX_SENSORS = 4U;

constexpr uint8_t SECTION_COUNT = 3U;

struct SectionDescriptor {
  uint8_t id;
  uint8_t output_pin;
  uint8_t switch_pin;
};

template <size_t N>
constexpr bool sectionIdsAreContiguous(const std::array<SectionDescriptor, N>& descriptors) {
  for (size_t i = 0U; i < N; ++i) {
    if (descriptors[i].id != i) {
      return false;
    }
  }
  return true;
}

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
constexpr uint32_t TELEMETRY_INTERVAL_MS = 200UL;
constexpr uint32_t PREVIEW_INTERVAL_MS = LOOP_INTERVAL_MS;
constexpr int TELEMETRY_MIN_TX_BUFFER_BYTES = 32;
constexpr uint32_t INPUT_DEBOUNCE_MS = 30UL;
constexpr uint8_t TELEMETRY_FRAME_BUDGET_PER_LOOP = 6U;
constexpr uint8_t SERIAL_INGRESS_BUDGET_BYTES_PER_LOOP = 24U;
constexpr uint8_t OPERATOR_EVENT_DEQUEUE_BUDGET_PER_LOOP = 3U;
constexpr size_t OPERATOR_EVENT_QUEUE_CAPACITY = 8U;
constexpr uint32_t INPUT_PHASE_BUDGET_MS = 15UL;
constexpr uint32_t CONTROL_PHASE_BUDGET_MS = 10UL;
constexpr uint32_t OUTPUT_PHASE_BUDGET_MS = 15UL;
constexpr uint32_t TELEMETRY_PHASE_BUDGET_MS = 20UL;

constexpr float FLOW_PULSES_PER_LITER = 450.0f;
constexpr uint32_t CALIBRATION_STORAGE_MAGIC = 0x53435259UL;
constexpr uint32_t FLOW_STALE_TIMEOUT_MS = SENSOR_FAULT_TIMEOUT_MS;
constexpr float FLOW_STALE_FALLBACK_LPM = 0.0f;
constexpr float WHEEL_CIRCUMFERENCE_M = 2.0f;
constexpr float WHEEL_PULSES_PER_REV = 20.0f;
constexpr uint32_t WHEEL_PULSE_TIMEOUT_MS = 1200UL;

constexpr bool ENABLE_PRESSURE_SENSOR = false;
constexpr bool ENABLE_LEGACY_PROTOCOL_PREFIXES = false;
constexpr float PRESSURE_SENSOR_MIN_KPA = 0.0f;
constexpr float PRESSURE_SENSOR_MAX_KPA = 1000.0f;

constexpr uint8_t SENSOR_COUNT =
    static_cast<uint8_t>(2U + (ENABLE_PRESSURE_SENSOR ? 1U : 0U));

static_assert(SECTION_COUNT <= MAX_SECTIONS,
              "SECTION_COUNT must not exceed MAX_SECTIONS.");
static_assert(SENSOR_COUNT <= MAX_SENSORS,
              "SENSOR_COUNT must not exceed MAX_SENSORS.");

}  // namespace spray

#endif  // SPRAY_CONTROLLER_CONFIG_H
