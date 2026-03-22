#ifndef SPRAY_CONTROLLER_PROTOCOL_H
#define SPRAY_CONTROLLER_PROTOCOL_H

#include <stdint.h>

namespace spray {

constexpr const char* MSG_STATUS_PREFIX = "ST:";
constexpr const char* MSG_PREVIEW_PREFIX = "PV:";
constexpr const char* MSG_FLOW_PREFIX = "FS:";
constexpr const char* MSG_PUMP_PREFIX = "P:";
constexpr const char* MSG_SECTION_PREFIX = "S:";
constexpr const char* MSG_SWITCH_PREFIX = "SW:";
constexpr const char* MSG_PRESSURE_PREFIX = "PR:";
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

}  // namespace spray

#endif  // SPRAY_CONTROLLER_PROTOCOL_H
