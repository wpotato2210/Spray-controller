#ifndef SPRAY_CONTROLLER_PROTOCOL_H
#define SPRAY_CONTROLLER_PROTOCOL_H

namespace spray {

constexpr const char* MSG_FLOW_PREFIX = "FS:";
constexpr const char* MSG_PUMP_PREFIX = "P:";
constexpr const char* MSG_SECTION_PREFIX = "S:";
constexpr const char* MSG_SWITCH_PREFIX = "SW:";
constexpr char MSG_TERMINATOR = '\n';

}  // namespace spray

#endif  // SPRAY_CONTROLLER_PROTOCOL_H
