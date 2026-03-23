#include <Arduino.h>
#include <string.h>

#include "config.h"
#include "arduino_adapters.h"
#include "calibration_store.h"
#include "interfaces.h"
#include "pins.h"
#include "protocol.h"
#include "interrupt_guards.h"
#include "operator_menu.h"

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

void setupPins() { g_section_hardware.begin(); }

void readSections() {
  for (const SectionDescriptor& section : kSectionDescriptors) {
    const bool is_enabled = g_section_hardware.readSwitch(section.id);
    g_section_manager.setSection(section.id, is_enabled);
  }
}

bool isSectionSwitchEnabled(uint8_t section_id) {
  for (const SectionDescriptor& section : kSectionDescriptors) {
    if (section.id == section_id) {
      return g_section_hardware.readSwitch(section.id);
    }
  }
  return false;
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


void publishSectionTelemetry() {
  for (const SectionDescriptor& section : kSectionDescriptors) {
    for (const TelemetrySectionFrameContract& frame : kSectionTelemetryFrameContract) {
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
  }
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

void publishSensorTelemetry(float flow_lpm, float speed_kmh) {
  for (const TelemetrySensorContract& sensor : kTelemetrySensorContracts) {
    if (!shouldEmitSensorContract(sensor.sensor_id)) {
      continue;
    }

    for (const TelemetrySensorFrameContract& frame : kSensorTelemetryFrameContract) {
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
  }
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
  return OperatorMenuEvent::kNone;
}

void processOperatorCommand(uint32_t now_ms) {
  static char command_buffer[16];
  static uint8_t command_length = 0U;
  while (Serial.available() > 0) {
    const char incoming = static_cast<char>(Serial.read());
    if (incoming == '\r') {
      continue;
    }
    if (incoming != '\n') {
      if (command_length < (sizeof(command_buffer) - 1U)) {
        command_buffer[command_length++] = incoming;
      } else {
        command_length = 0U;
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
    if (g_operator_menu.update(now_ms, event)) {
      publishMenuState(g_operator_menu.getState());
    }
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
}  // namespace
}  // namespace spray

void setup() {
  Serial.begin(115200);
  if (!spray::interruptPinsValid()) {
    Serial.println("ERR: Interrupt pin configuration invalid for FLOW/WHEEL sensors.");
    while (true) {
      delay(1000);
    }
  }
  spray::setupPins();
  spray::calibrationStore().begin();
  spray::calibrationStore().load();
  spray::g_flow_sensor.begin();
  spray::g_wheel_sensor.begin();
  spray::g_run_hold.begin();
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
  const uint32_t now_ms = millis();
  spray::processOperatorCommand(now_ms);
  spray::g_operator_menu.update(now_ms, spray::OperatorMenuEvent::kNone);
  spray::executeResetIfConfirmed();
  if ((now_ms - last_loop_ms) < spray::LOOP_INTERVAL_MS) {
    return;
  }
  last_loop_ms = now_ms;

  spray::readSections();

  const float measured_flow_lpm = spray::g_flow_sensor.readFlow();
  const float speed_kmh = spray::g_wheel_sensor.readSpeed();
  const bool run_enabled = spray::g_run_hold.readRunHold();

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
  if (spray::shouldPublishPreview(now_ms, last_preview_ms) && spray::hasTelemetryTxCapacity()) {
    spray::publishPreview(speed_kmh, measured_flow_lpm, duty, active_sections, distance_m, area_ha);
    last_preview_ms = now_ms;
  }
  if (spray::shouldPublishTelemetry(now_ms, last_telemetry_ms) && spray::hasTelemetryTxCapacity()) {
    spray::publishStatus(measured_flow_lpm, duty, run_enabled, spray::getStatusFaultBitfield());
    spray::publishSectionTelemetry();
    spray::publishSensorTelemetry(measured_flow_lpm, speed_kmh);
#if ENABLE_PRESSURE_SENSOR
    spray::publishPressure(spray::g_pressure_sensor.readPressure());
#endif
    last_telemetry_ms = now_ms;
  }
}
