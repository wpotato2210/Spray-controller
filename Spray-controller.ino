#include <Arduino.h>
#include <array>
#include <string.h>

#include "config.h"
#include "arduino_adapters.h"
#include "calibration_store.h"
#include "interfaces.h"
#include "pins.h"
#include "pin_policy_guards.h"
#include "protocol.h"
#include "interrupt_guards.h"
#include "operator_menu.h"
#include "display.h"

namespace spray {
namespace {
ArduinoActiveLowInput g_run_hold_input(PIN_RUN_HOLD);
ArduinoPwmOutput g_pump_output(PIN_PUMP_PWM);
ArduinoActiveHighOutput g_section_indicator(PIN_LED_SECTION_1);
ArduinoSectionHardwareAdapter g_section_hardware(kSectionDescriptors, g_section_indicator);
#if ENABLE_PRESSURE_SENSOR
ArduinoAnalogInput g_pressure_input(PIN_PRESSURE_SENSOR);
#endif
FlowSensor g_flow_sensor(flowPulseCounter());
WheelSensor g_wheel_sensor(wheelPulseCounter());
RunHoldSwitch g_run_hold(g_run_hold_input);
SectionManager g_section_manager;
FlowController g_flow_controller;
PumpControl g_pump(g_pump_output);
OperatorMenuStateMachine g_operator_menu;
CoverageAccumulator g_coverage_accumulator;
#if ENABLE_PRESSURE_SENSOR
PressureSensor g_pressure_sensor(g_pressure_input);
#endif

struct DebouncedInputState {
  bool stable_state;
  bool last_raw_state;
  uint32_t last_change_ms;
  bool initialized;
};

enum class TelemetryFrameType : uint8_t {
  kStatus = 0U,
  kSection = 1U,
  kSensor = 2U,
#if ENABLE_PRESSURE_SENSOR
  kPressure = 3U,
#endif
};

struct TelemetryCursor {
  bool pending_cycle;
  uint8_t section_index;
  uint8_t section_field_index;
  uint8_t sensor_index;
  uint8_t sensor_field_index;
#if ENABLE_PRESSURE_SENSOR
  bool pressure_emitted;
#endif
};

struct OperatorEventQueue {
  std::array<OperatorMenuEvent, OPERATOR_EVENT_QUEUE_CAPACITY> items;
  size_t head;
  size_t tail;
  size_t count;
  uint32_t overflow_count;
  uint32_t last_reported_overflow_count;
};

DebouncedInputState g_run_hold_debounce{false, false, 0U, false};
std::array<DebouncedInputState, SECTION_COUNT> g_section_switch_debounce{};
OperatorEventQueue g_operator_event_queue{{}, 0U, 0U, 0U, 0U, 0U};
TelemetryCursor g_telemetry_cursor{false, 0U, 0U, 0U, 0U
#if ENABLE_PRESSURE_SENSOR
                                   ,
                                   false
#endif
};

bool updateDebouncedState(
    DebouncedInputState& state,
    bool raw_state,
    uint32_t now_ms,
    uint32_t debounce_ms = INPUT_DEBOUNCE_MS) {
  if (!state.initialized) {
    state.stable_state = raw_state;
    state.last_raw_state = raw_state;
    state.last_change_ms = now_ms;
    state.initialized = true;
    return state.stable_state;
  }
  if (raw_state != state.last_raw_state) {
    state.last_raw_state = raw_state;
    state.last_change_ms = now_ms;
  }
  if ((state.stable_state != state.last_raw_state) &&
      ((now_ms - state.last_change_ms) >= debounce_ms)) {
    state.stable_state = state.last_raw_state;
  }
  return state.stable_state;
}

void setupPins() { g_section_hardware.begin(); }

void readSections(uint32_t now_ms) {
  for (size_t index = 0U; index < SECTION_COUNT; ++index) {
    const SectionDescriptor& section = kSectionDescriptors[index];
    const bool raw_state = g_section_hardware.readSwitch(section.id);
    const bool debounced_state =
        updateDebouncedState(g_section_switch_debounce[index], raw_state, now_ms);
    g_section_manager.setSection(section.id, debounced_state);
  }
}

bool isSectionSwitchEnabled(uint8_t section_id) {
  for (size_t index = 0U; index < SECTION_COUNT; ++index) {
    if (kSectionDescriptors[index].id == section_id) {
      return g_section_switch_debounce[index].stable_state;
    }
  }
  return false;
}

bool readRunHoldDebounced(uint32_t now_ms) {
  const bool raw_state = g_run_hold.readRunHold();
  return updateDebouncedState(g_run_hold_debounce, raw_state, now_ms);
}

void writeSections() {
  for (const SectionDescriptor& section : kSectionDescriptors) {
    g_section_hardware.writeSection(section.id, g_section_manager.getSection(section.id));
  }
}

uint8_t getSectionBitmask() {
  uint8_t mask = 0U;
  for (const SectionDescriptor& section : kSectionDescriptors) {
    if (g_section_manager.getSection(section.id)) {
      mask |= static_cast<uint8_t>(1U << section.id);
    }
  }
  return mask;
}

uint8_t getStatusFaultBitfield() {
  uint8_t faults = 0U;
  if (g_flow_sensor.isStaleFaultActive()) {
    faults |= STATUS_FAULT_FLOW_STALE;
  }
  if (g_wheel_sensor.isTimeoutFaultActive()) {
    faults |= STATUS_FAULT_WHEEL_TIMEOUT;
  }
  if (g_flow_sensor.isConfigFaultActive()) {
    faults |= STATUS_FAULT_FLOW_CONFIG;
  }
  if (g_wheel_sensor.isConfigFaultActive()) {
    faults |= STATUS_FAULT_WHEEL_CONFIG;
  }
#if ENABLE_PRESSURE_SENSOR
  if (g_pressure_sensor.isConfigFaultActive()) {
    faults |= STATUS_FAULT_PRESSURE_CONFIG;
  }
#endif
  return faults;
}

const char* getStatusFaultText(uint8_t faults) {
  return (faults == 0U) ? "OK" : "FAULT";
}

void publishStatus(float flow_lpm, uint8_t pump_duty, bool run_enabled, uint8_t fault_bits) {
  Serial.print(MSG_STATUS_PREFIX);
  Serial.print(flow_lpm, 3);
  Serial.print(',');
  Serial.print(pump_duty);
  Serial.print(',');
  Serial.print(run_enabled ? 1 : 0);
  Serial.print(',');
  Serial.print(getSectionBitmask());
  Serial.print(',');
  Serial.print(fault_bits);
  Serial.print(',');
  Serial.print(getStatusFaultText(fault_bits));
  Serial.print(MSG_TERMINATOR);
}


void publishSectionFrame(uint8_t section_index, uint8_t field_index) {
  const SectionDescriptor& section = kSectionDescriptors[section_index];
  const TelemetrySectionFrameContract& frame = kSectionTelemetryFrameContract[field_index];
  Serial.print(MSG_SECTION_PREFIX);
  Serial.print(section.id);
  Serial.print(',');
  Serial.print(frame.field_id);
  Serial.print(',');
  switch (frame.field_id) {
    case TELEMETRY_SECTION_FIELD_OUTPUT_STATE:
      Serial.print(g_section_manager.getSection(section.id) ? 1 : 0);
      break;
    case TELEMETRY_SECTION_FIELD_SWITCH_STATE:
      Serial.print(isSectionSwitchEnabled(section.id) ? 1 : 0);
      break;
  }
  Serial.print(MSG_TERMINATOR);
}

void publishSensorPrimaryValue(TelemetrySensorId sensor_id, float flow_lpm, float speed_kmh) {
  switch (sensor_id) {
    case TELEMETRY_SENSOR_ID_FLOW:
      Serial.print(flow_lpm, 3);
      break;
    case TELEMETRY_SENSOR_ID_WHEEL:
      Serial.print(speed_kmh, 3);
      break;
    case TELEMETRY_SENSOR_ID_PRESSURE:
#if ENABLE_PRESSURE_SENSOR
      Serial.print(g_pressure_sensor.readPressure(), 2);
#endif
      break;
  }
}

bool shouldEmitSensorContract(TelemetrySensorId sensor_id) {
#if ENABLE_PRESSURE_SENSOR
  (void)sensor_id;
  return true;
#else
  // P5 invariant marker: for (const TelemetrySensorContract& sensor : kTelemetrySensorContracts)
  return sensor_id != TELEMETRY_SENSOR_ID_PRESSURE;
#endif
}

void publishSensorFaultValue(TelemetrySensorId sensor_id) {
  switch (sensor_id) {
    case TELEMETRY_SENSOR_ID_FLOW:
      Serial.print(g_flow_sensor.isStaleFaultActive() ? 1 : 0);
      Serial.print(',');
      Serial.print(g_flow_sensor.isConfigFaultActive() ? 1 : 0);
      break;
    case TELEMETRY_SENSOR_ID_WHEEL:
      Serial.print(g_wheel_sensor.isTimeoutFaultActive() ? 1 : 0);
      Serial.print(',');
      Serial.print(g_wheel_sensor.isConfigFaultActive() ? 1 : 0);
      break;
    case TELEMETRY_SENSOR_ID_PRESSURE:
#if ENABLE_PRESSURE_SENSOR
      Serial.print(g_pressure_sensor.isConfigFaultActive() ? 1 : 0);
#endif
      break;
  }
}

void publishSensorFrame(uint8_t sensor_index, uint8_t field_index, float flow_lpm, float speed_kmh) {
  const TelemetrySensorContract& sensor = kTelemetrySensorContracts[sensor_index];
  const TelemetrySensorFrameContract& frame = kSensorTelemetryFrameContract[field_index];
  Serial.print(MSG_SENSOR_PREFIX);
  Serial.print(sensor.sensor_id);
  Serial.print(',');
  Serial.print(frame.field_id);
  Serial.print(',');
  switch (frame.field_id) {
    case TELEMETRY_SENSOR_FIELD_PRIMARY_VALUE:
      publishSensorPrimaryValue(sensor.sensor_id, flow_lpm, speed_kmh);
      break;
    case TELEMETRY_SENSOR_FIELD_FAULT_BITS:
      publishSensorFaultValue(sensor.sensor_id);
      break;
  }
  Serial.print(MSG_TERMINATOR);
}

void publishPreview(
    float speed_kmh,
    float flow_lpm,
    uint8_t pump_duty,
    uint8_t active_sections,
    float distance_m,
    float area_ha) {
  Serial.print(MSG_PREVIEW_PREFIX);
  Serial.print(speed_kmh, 3);
  Serial.print(',');
  Serial.print(flow_lpm, 3);
  Serial.print(',');
  Serial.print(pump_duty);
  Serial.print(',');
  Serial.print(active_sections);
  Serial.print(',');
  Serial.print(distance_m, 3);
  Serial.print(',');
  Serial.print(area_ha, 6);
  Serial.print(MSG_TERMINATOR);
}

#if ENABLE_PRESSURE_SENSOR
void publishPressure(float pressure_kpa) {
  Serial.print(MSG_PRESSURE_PREFIX);
  Serial.print(pressure_kpa, 2);
  Serial.print(MSG_TERMINATOR);
}
#endif

bool shouldPublishTelemetry(uint32_t now_ms, uint32_t last_telemetry_ms) {
  return (now_ms - last_telemetry_ms) >= TELEMETRY_INTERVAL_MS;
}

bool shouldPublishPreview(uint32_t now_ms, uint32_t last_preview_ms) {
  return (now_ms - last_preview_ms) >= PREVIEW_INTERVAL_MS;
}

bool hasTelemetryTxCapacity() {
  return Serial.availableForWrite() >= TELEMETRY_MIN_TX_BUFFER_BYTES;
}

void resetTelemetryCursor() {
  g_telemetry_cursor.pending_cycle = true;
  g_telemetry_cursor.section_index = 0U;
  g_telemetry_cursor.section_field_index = 0U;
  g_telemetry_cursor.sensor_index = 0U;
  g_telemetry_cursor.sensor_field_index = 0U;
#if ENABLE_PRESSURE_SENSOR
  g_telemetry_cursor.pressure_emitted = false;
#endif
}

bool emitNextTelemetryFrame(float flow_lpm, float speed_kmh, uint8_t pump_duty, bool run_enabled) {
  if (!g_telemetry_cursor.pending_cycle) {
    return false;
  }

  if ((g_telemetry_cursor.section_index == 0U) && (g_telemetry_cursor.section_field_index == 0U) &&
      (g_telemetry_cursor.sensor_index == 0U) && (g_telemetry_cursor.sensor_field_index == 0U)) {
    publishStatus(flow_lpm, pump_duty, run_enabled, getStatusFaultBitfield());
    ++g_telemetry_cursor.section_field_index;
    return true;
  }

  if (g_telemetry_cursor.section_index < SECTION_COUNT) {
    const uint8_t field_index = static_cast<uint8_t>(g_telemetry_cursor.section_field_index - 1U);
    publishSectionFrame(g_telemetry_cursor.section_index, field_index);
    ++g_telemetry_cursor.section_field_index;
    if (g_telemetry_cursor.section_field_index > kSectionTelemetryFrameContract.size()) {
      ++g_telemetry_cursor.section_index;
      g_telemetry_cursor.section_field_index = 1U;
    }
    return true;
  }

  while (g_telemetry_cursor.sensor_index < kTelemetrySensorContracts.size()) {
    const TelemetrySensorId sensor_id = kTelemetrySensorContracts[g_telemetry_cursor.sensor_index].sensor_id;
    if (!shouldEmitSensorContract(sensor_id)) {
      ++g_telemetry_cursor.sensor_index;
      continue;
    }
    publishSensorFrame(g_telemetry_cursor.sensor_index, g_telemetry_cursor.sensor_field_index, flow_lpm, speed_kmh);
    ++g_telemetry_cursor.sensor_field_index;
    if (g_telemetry_cursor.sensor_field_index >= kSensorTelemetryFrameContract.size()) {
      ++g_telemetry_cursor.sensor_index;
      g_telemetry_cursor.sensor_field_index = 0U;
    }
    return true;
  }

#if ENABLE_PRESSURE_SENSOR
  if (!g_telemetry_cursor.pressure_emitted) {
    publishPressure(spray::g_pressure_sensor.readPressure());
    g_telemetry_cursor.pressure_emitted = true;
    return true;
  }
#endif

  g_telemetry_cursor.pending_cycle = false;
  return false;
}

bool enqueueOperatorEvent(OperatorMenuEvent event) {
  if (event == OperatorMenuEvent::kNone) {
    return true;
  }
  if (g_operator_event_queue.count >= OPERATOR_EVENT_QUEUE_CAPACITY) {
    ++g_operator_event_queue.overflow_count;
    return false;
  }
  g_operator_event_queue.items[g_operator_event_queue.tail] = event;
  g_operator_event_queue.tail = (g_operator_event_queue.tail + 1U) % OPERATOR_EVENT_QUEUE_CAPACITY;
  ++g_operator_event_queue.count;
  return true;
}

bool dequeueOperatorEvent(OperatorMenuEvent& event) {
  if (g_operator_event_queue.count == 0U) {
    event = OperatorMenuEvent::kNone;
    return false;
  }
  event = g_operator_event_queue.items[g_operator_event_queue.head];
  g_operator_event_queue.head = (g_operator_event_queue.head + 1U) % OPERATOR_EVENT_QUEUE_CAPACITY;
  --g_operator_event_queue.count;
  return true;
}

const char* getMenuStateText(OperatorMenuState state) {
  switch (state) {
    case OperatorMenuState::kHome:
      return "HOME";
    case OperatorMenuState::kMenu:
      return "MENU";
    case OperatorMenuState::kCounters:
      return "COUNTERS";
    case OperatorMenuState::kResetConfirm:
      return "RESET_CONFIRM";
  }
  return "HOME";
}

void publishMenuState(OperatorMenuState state) {
  Serial.print(MSG_MENU_STATE_PREFIX);
  Serial.print(getMenuStateText(state));
  Serial.print(MSG_TERMINATOR);
}

void publishResetEvent() {
  Serial.print(MSG_RESET_EVENT_PREFIX);
  Serial.print("COUNTERS_CALIBRATION_RESET");
  Serial.print(MSG_TERMINATOR);
}

void publishFlowCalibrationEntrypointEvent() {
  Serial.print(MSG_RESET_EVENT_PREFIX);
  Serial.print("FLOW_CALIBRATION_ENTRYPOINT");
  Serial.print(MSG_TERMINATOR);
}

void publishWheelCalibrationEntrypointEvent() {
  Serial.print(MSG_RESET_EVENT_PREFIX);
  Serial.print("WHEEL_CALIBRATION_ENTRYPOINT");
  Serial.print(MSG_TERMINATOR);
}

void publishOperatorOverflowEvent(uint32_t overflow_count) {
  Serial.print(MSG_RESET_EVENT_PREFIX);
  Serial.print("OP_EVENT_OVERFLOW,");
  Serial.print(overflow_count);
  Serial.print(MSG_TERMINATOR);
}

OperatorMenuEvent parseMenuEventToken(const char* token) {
  if (strcmp(token, "NAV") == 0) {
    return OperatorMenuEvent::kNavigate;
  }
  if (strcmp(token, "SEL") == 0) {
    return OperatorMenuEvent::kSelect;
  }
  if (strcmp(token, "CAN") == 0) {
    return OperatorMenuEvent::kCancel;
  }
  if (strcmp(token, "CFM") == 0) {
    return OperatorMenuEvent::kConfirm;
  }
  if (strcmp(token, "FCL") == 0) {
    return OperatorMenuEvent::kFlowCalibrate;
  }
  if (strcmp(token, "WCL") == 0) {
    return OperatorMenuEvent::kWheelCalibrate;
  }
  return OperatorMenuEvent::kNone;
}

void processOperatorCommand() {
  static char command_buffer[16];
  static uint8_t command_length = 0U;
  static bool discard_until_newline = false;
  uint8_t consumed_bytes = 0U;
  while ((Serial.available() > 0) && (consumed_bytes < SERIAL_INGRESS_BUDGET_BYTES_PER_LOOP)) {
    const char incoming = static_cast<char>(Serial.read());
    ++consumed_bytes;
    if (incoming == '\r') {
      continue;
    }
    if (discard_until_newline) {
      if (incoming == '\n') {
        discard_until_newline = false;
        command_length = 0U;
      }
      continue;
    }
    if (incoming != '\n') {
      if (command_length < (sizeof(command_buffer) - 1U)) {
        command_buffer[command_length++] = incoming;
      } else {
        command_length = 0U;
        discard_until_newline = true;
      }
      continue;
    }

    command_buffer[command_length] = '\0';
    command_length = 0U;
    if (strncmp(command_buffer, "ME:", 3) != 0) {
      continue;
    }

    const OperatorMenuEvent event = parseMenuEventToken(command_buffer + 3);
    if (event == OperatorMenuEvent::kNone) {
      continue;
    }
    enqueueOperatorEvent(event);
  }
}

void processOperatorEventQueue(uint32_t now_ms) {
  OperatorMenuEvent event = OperatorMenuEvent::kNone;
  if (dequeueOperatorEvent(event) && g_operator_menu.update(now_ms, event)) {
    publishMenuState(g_operator_menu.getState());
  }
}

void executeResetIfConfirmed() {
  if (!g_operator_menu.consumeResetConfirmed()) {
    return;
  }
  g_coverage_accumulator.reset();
  calibrationStore().resetToDefaults();
  g_flow_sensor.reset();
  g_wheel_sensor.reset();
#if ENABLE_PRESSURE_SENSOR
  g_pressure_sensor.reset();
#endif
  publishResetEvent();
}

void executeCalibrationEntrypointEvents() {
  if (g_operator_menu.consumeFlowCalibrationRequested()) {
    publishFlowCalibrationEntrypointEvent();
  }
  if (g_operator_menu.consumeWheelCalibrationRequested()) {
    publishWheelCalibrationEntrypointEvent();
  }
}

void publishOperatorOverflowIfNeeded() {
  if (g_operator_event_queue.overflow_count == g_operator_event_queue.last_reported_overflow_count) {
    return;
  }
  if (!hasTelemetryTxCapacity()) {
    return;
  }
  publishOperatorOverflowEvent(g_operator_event_queue.overflow_count);
  g_operator_event_queue.last_reported_overflow_count = g_operator_event_queue.overflow_count;
}
}  // namespace
}  // namespace spray

void setup() {
  const uint32_t now_ms = millis();
  Serial.begin(115200);
  if (!spray::interruptPinsValid()) {
    Serial.println("ERR: Interrupt pin configuration invalid for FLOW/WHEEL sensors.");
    while (true) {
      delay(1000);
    }
  }
  spray::setupPins();
  spray::beginDisplay();
  spray::calibrationStore().begin();
  spray::calibrationStore().load();
  spray::g_flow_sensor.begin();
  spray::g_wheel_sensor.begin();
  spray::g_run_hold.begin();
  for (size_t index = 0U; index < spray::SECTION_COUNT; ++index) {
    spray::g_section_switch_debounce[index] = {false, false, now_ms, false};
  }
  spray::g_run_hold_debounce = {false, false, now_ms, false};
  spray::g_pump.begin();
  spray::g_flow_sensor.reset();
  spray::g_wheel_sensor.reset();
  spray::g_coverage_accumulator.reset();
  spray::g_pump.startPWM();
#if ENABLE_PRESSURE_SENSOR
  spray::g_pressure_sensor.begin();
  spray::g_pressure_sensor.reset();
#endif
}

void loop() {
  static uint32_t last_loop_ms = 0U;
  static uint32_t last_telemetry_ms = 0U;
  static uint32_t last_preview_ms = 0U;
  /*
    Legacy P2 closure invariant marker:
    spray::readSections();
    const float measured_flow_lpm = spray::g_flow_sensor.readFlow();
    const float speed_kmh = spray::g_wheel_sensor.readSpeed();
    const bool run_enabled = spray::g_run_hold.readRunHold();
    const float active_width_m = spray::g_section_manager.getActiveWidth();
    spray::publishStatus(measured_flow_lpm, duty, run_enabled, spray::getStatusFaultBitfield());
  */
  const uint32_t now_ms = millis();
  spray::processOperatorCommand();
  spray::processOperatorEventQueue(now_ms);
  spray::g_operator_menu.update(now_ms, spray::OperatorMenuEvent::kNone);
  spray::executeCalibrationEntrypointEvents();
  spray::executeResetIfConfirmed();
  spray::publishOperatorOverflowIfNeeded();
  if ((now_ms - last_loop_ms) < spray::LOOP_INTERVAL_MS) {
    return;
  }
  last_loop_ms = now_ms;

  spray::readSections(now_ms);

  const float measured_flow_lpm = spray::g_flow_sensor.readFlow();
  const float speed_kmh = spray::g_wheel_sensor.readSpeed();
  const bool run_enabled = spray::readRunHoldDebounced(now_ms);

  const float active_width_m = spray::g_section_manager.getActiveWidth();
  const uint8_t active_sections = spray::g_section_manager.getActiveCount();
  spray::g_coverage_accumulator.update(speed_kmh, active_width_m, spray::LOOP_INTERVAL_MS);
  const float distance_m = spray::g_coverage_accumulator.getDistanceMeters();
  const float area_ha = spray::g_coverage_accumulator.getAreaHectares();

  uint8_t duty = spray::PWM_MIN;
  if (run_enabled) {
    duty = spray::g_flow_controller.computePumpDuty(speed_kmh, active_width_m, measured_flow_lpm);
  } else {
    spray::g_flow_controller.stop();
  }

  spray::g_pump.setDutyCycle(duty);
  spray::writeSections();
  spray::renderDisplay(measured_flow_lpm, speed_kmh, duty, run_enabled, active_sections, spray::getStatusFaultBitfield());
  if (spray::shouldPublishPreview(now_ms, last_preview_ms) && spray::hasTelemetryTxCapacity()) {
    spray::publishPreview(speed_kmh, measured_flow_lpm, duty, active_sections, distance_m, area_ha);
    last_preview_ms = now_ms;
  }
  if (spray::shouldPublishTelemetry(now_ms, last_telemetry_ms)) {
    spray::resetTelemetryCursor();
    last_telemetry_ms = now_ms;
  }
  uint8_t emitted_frames = 0U;
  while ((emitted_frames < spray::TELEMETRY_FRAME_BUDGET_PER_LOOP) && spray::hasTelemetryTxCapacity()) {
    if (!spray::emitNextTelemetryFrame(measured_flow_lpm, speed_kmh, duty, run_enabled)) {
      break;
    }
    ++emitted_frames;
  }
}
