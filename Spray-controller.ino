#include <Arduino.h>

#include "config.h"
#include "interfaces.h"
#include "pins.h"
#include "protocol.h"

namespace spray {
namespace {
FlowSensor g_flow_sensor(PIN_FLOW_SENSOR);
WheelSensor g_wheel_sensor(PIN_WHEEL_SENSOR);
RunHoldSwitch g_run_hold(PIN_RUN_HOLD);
SectionManager g_section_manager;
FlowController g_flow_controller;
PumpControl g_pump(PIN_PUMP_PWM);

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

void publishStatus(float flow_lpm, uint8_t pump_duty, bool run_enabled) {
  Serial.print(MSG_FLOW_PREFIX);
  Serial.print(flow_lpm, 3);
  Serial.print(MSG_TERMINATOR);

  Serial.print(MSG_PUMP_PREFIX);
  Serial.print(pump_duty);
  Serial.print(MSG_TERMINATOR);

  for (uint8_t i = 0U; i < SECTION_COUNT; ++i) {
    Serial.print(MSG_SECTION_PREFIX);
    Serial.print(i);
    Serial.print(':');
    Serial.print(g_section_manager.getSection(i) ? 1 : 0);
    Serial.print(MSG_TERMINATOR);
  }

  Serial.print(MSG_SWITCH_PREFIX);
  Serial.print(run_enabled ? 1 : 0);
  Serial.print(MSG_TERMINATOR);
}
}  // namespace
}  // namespace spray

void setup() {
  Serial.begin(115200);
  spray::setupPins();
  spray::g_flow_sensor.begin();
  spray::g_wheel_sensor.begin();
  spray::g_run_hold.begin();
  spray::g_pump.begin();
  spray::g_flow_sensor.reset();
  spray::g_wheel_sensor.reset();
  spray::g_pump.startPWM();
}

void loop() {
  static uint32_t last_loop_ms = 0U;
  const uint32_t now_ms = millis();
  if ((now_ms - last_loop_ms) < spray::LOOP_INTERVAL_MS) {
    return;
  }
  last_loop_ms = now_ms;

  spray::readSections();

  const float measured_flow_lpm = spray::g_flow_sensor.readFlow();
  const float speed_kmh = spray::g_wheel_sensor.readSpeed();
  const bool run_enabled = spray::g_run_hold.readRunHold();

  const float active_width_m = spray::g_section_manager.getActiveWidth();

  uint8_t duty = spray::PWM_MIN;
  if (run_enabled) {
    duty = spray::g_flow_controller.computePumpDuty(speed_kmh, active_width_m, measured_flow_lpm);
  } else {
    spray::g_flow_controller.stop();
  }

  spray::g_pump.setDutyCycle(duty);
  spray::writeSections();
  spray::publishStatus(measured_flow_lpm, duty, run_enabled);
}
