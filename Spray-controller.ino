#include <Arduino.h>

#include "config.h"
#include "interfaces.h"
#include "pins.h"
#include "protocol.h"
#include "interrupt_guards.h"
#include "operator_menu.h"

namespace spray {
namespace {
FlowSensor g_flow_sensor(PIN_FLOW_SENSOR);
WheelSensor g_wheel_sensor(PIN_WHEEL_SENSOR);
RunHoldSwitch g_run_hold(PIN_RUN_HOLD);
SectionManager g_section_manager;
FlowController g_flow_controller;
PumpControl g_pump(PIN_PUMP_PWM);
OperatorMenuStateMachine g_operator_menu;
#if ENABLE_PRESSURE_SENSOR
PressureSensor g_pressure_sensor(PIN_PRESSURE_SENSOR);
#endif

const uint8_t kSectionOutputPins[SECTION_COUNT] = {PIN_BOOM_1, PIN_BOOM_2, PIN_BOOM_3};
const uint8_t kSectionSwitchPins[SECTION_COUNT] = {
    PIN_SECTION_SW_1, PIN_SECTION_SW_2, PIN_SECTION_SW_3};

void setupPins() {
  for (uint8_t i = 0U; i < SECTION_COUNT; ++i) {
    pinMode(kSectionOutputPins[i], OUTPUT);
    pinMode(kSectionSwitchPins[i], INPUT_PULLUP);
    digitalWrite(kSectionOutputPins[i], LOW);
  }
  pinMode(PIN_LED_SECTION_1, OUTPUT);
  digitalWrite(PIN_LED_SECTION_1, LOW);
}

void readSections() {
  for (uint8_t i = 0U; i < SECTION_COUNT; ++i) {
    const bool is_enabled = (digitalRead(kSectionSwitchPins[i]) == LOW);
    g_section_manager.setSection(i, is_enabled);
  }
}

void writeSections() {
  for (uint8_t i = 0U; i < SECTION_COUNT; ++i) {
    digitalWrite(kSectionOutputPins[i], g_section_manager.getSection(i) ? HIGH : LOW);
  }
  digitalWrite(PIN_LED_SECTION_1, g_section_manager.getSection(0U) ? HIGH : LOW);
}

uint8_t getSectionBitmask() {
  uint8_t mask = 0U;
  for (uint8_t i = 0U; i < SECTION_COUNT; ++i) {
    if (g_section_manager.getSection(i)) {
      mask |= static_cast<uint8_t>(1U << i);
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
  spray::g_flow_sensor.begin();
  spray::g_wheel_sensor.begin();
  spray::g_run_hold.begin();
  spray::g_pump.begin();
  spray::g_flow_sensor.reset();
  spray::g_wheel_sensor.reset();
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
  spray::g_operator_menu.update(now_ms, spray::OperatorMenuEvent::kNone);
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
  const float distance_m = 0.0f;
  const float area_ha = 0.0f;

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
#if ENABLE_PRESSURE_SENSOR
    spray::publishPressure(spray::g_pressure_sensor.readPressure());
#endif
    last_telemetry_ms = now_ms;
  }
}
