#ifndef SPRAY_CONTROLLER_PROTOCOL_H
#define SPRAY_CONTROLLER_PROTOCOL_H

#include <array>
#include <stdint.h>

namespace spray {

constexpr const char* MSG_STATUS_PREFIX = "ST:";
constexpr const char* MSG_PREVIEW_PREFIX = "PV:";
constexpr const char* MSG_FLOW_PREFIX = "FS:";
constexpr const char* MSG_PUMP_PREFIX = "P:";
constexpr const char* MSG_SECTION_PREFIX = "S:";
constexpr const char* MSG_SWITCH_PREFIX = "SW:";
constexpr const char* MSG_PRESSURE_PREFIX = "PR:";
constexpr const char* MSG_SENSOR_PREFIX = "SN:";
constexpr const char* MSG_MENU_STATE_PREFIX = "MS:";
constexpr const char* MSG_RESET_EVENT_PREFIX = "RS:";
constexpr char MSG_TERMINATOR = '\n';

enum StatusFaultBit : uint8_t {
  STATUS_FAULT_FLOW_STALE = 1U << 0,
  STATUS_FAULT_WHEEL_TIMEOUT = 1U << 1,
  STATUS_FAULT_FLOW_CONFIG = 1U << 2,
  STATUS_FAULT_WHEEL_CONFIG = 1U << 3,
  STATUS_FAULT_PRESSURE_CONFIG = 1U << 4
};

enum TelemetrySectionField : uint8_t {
  TELEMETRY_SECTION_FIELD_OUTPUT_STATE = 0U,
  TELEMETRY_SECTION_FIELD_SWITCH_STATE = 1U
};

struct TelemetrySectionFrameContract {
  TelemetrySectionField field_id;
};

constexpr std::array<TelemetrySectionFrameContract, 2U> kSectionTelemetryFrameContract = {{
    {TELEMETRY_SECTION_FIELD_OUTPUT_STATE},
    {TELEMETRY_SECTION_FIELD_SWITCH_STATE},
}};

enum TelemetrySensorId : uint8_t {
  TELEMETRY_SENSOR_ID_FLOW = 0U,
  TELEMETRY_SENSOR_ID_WHEEL = 1U,
  TELEMETRY_SENSOR_ID_PRESSURE = 2U
};

enum TelemetrySensorField : uint8_t {
  TELEMETRY_SENSOR_FIELD_PRIMARY_VALUE = 0U,
  TELEMETRY_SENSOR_FIELD_FAULT_BITS = 1U
};

struct TelemetrySensorContract {
  TelemetrySensorId sensor_id;
};

struct TelemetrySensorFrameContract {
  TelemetrySensorField field_id;
};

constexpr std::array<TelemetrySensorContract, 3U> kTelemetrySensorContracts = {{
    {TELEMETRY_SENSOR_ID_FLOW},
    {TELEMETRY_SENSOR_ID_WHEEL},
    {TELEMETRY_SENSOR_ID_PRESSURE},
}};

constexpr std::array<TelemetrySensorFrameContract, 2U> kSensorTelemetryFrameContract = {{
    {TELEMETRY_SENSOR_FIELD_PRIMARY_VALUE},
    {TELEMETRY_SENSOR_FIELD_FAULT_BITS},
}};

static_assert(kSectionTelemetryFrameContract[0].field_id == TELEMETRY_SECTION_FIELD_OUTPUT_STATE,
              "Section telemetry field ordering must remain stable.");
static_assert(kSectionTelemetryFrameContract[1].field_id == TELEMETRY_SECTION_FIELD_SWITCH_STATE,
              "Section telemetry field ordering must remain stable.");
static_assert(kTelemetrySensorContracts[0].sensor_id == TELEMETRY_SENSOR_ID_FLOW,
              "Sensor telemetry ordering must remain stable.");
static_assert(kTelemetrySensorContracts[1].sensor_id == TELEMETRY_SENSOR_ID_WHEEL,
              "Sensor telemetry ordering must remain stable.");
static_assert(kTelemetrySensorContracts[2].sensor_id == TELEMETRY_SENSOR_ID_PRESSURE,
              "Sensor telemetry ordering must remain stable.");
static_assert(kSensorTelemetryFrameContract[0].field_id == TELEMETRY_SENSOR_FIELD_PRIMARY_VALUE,
              "Sensor telemetry field ordering must remain stable.");
static_assert(kSensorTelemetryFrameContract[1].field_id == TELEMETRY_SENSOR_FIELD_FAULT_BITS,
              "Sensor telemetry field ordering must remain stable.");

}  // namespace spray

#endif  // SPRAY_CONTROLLER_PROTOCOL_H
