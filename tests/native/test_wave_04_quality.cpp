#include <cmath>
#include <cstdint>
#include <iostream>

#include "config.h"
#include "interfaces.h"

namespace spray {
float activeFlowPulsesPerLiter() { return FLOW_PULSES_PER_LITER; }
float activeWheelCircumferenceM() { return WHEEL_CIRCUMFERENCE_M; }
float activeWheelPulsesPerRev() { return WHEEL_PULSES_PER_REV; }
}  // namespace spray

namespace {

class FakeTime final : public spray::TimeSourceAdapter {
 public:
  uint32_t nowMs() const override { return now_ms_; }
  void setNowMs(uint32_t now_ms) { now_ms_ = now_ms; }

 private:
  uint32_t now_ms_ = 0;
};

class FakePulseCounter final : public spray::PulseCounterAdapter {
 public:
  void begin() override {}
  uint32_t readCount() const override { return count_; }
  uint32_t readLastPulseMs() const override { return last_pulse_ms_; }
  void reset() override {
    count_ = 0;
    last_pulse_ms_ = 0;
  }
  void setState(uint32_t count, uint32_t last_pulse_ms) {
    count_ = count;
    last_pulse_ms_ = last_pulse_ms;
  }

 private:
  uint32_t count_ = 0;
  uint32_t last_pulse_ms_ = 0;
};

bool nearlyEqual(float a, float b, float epsilon = 0.001f) {
  return std::fabs(a - b) <= epsilon;
}

void assertTrue(bool condition, const char* message) {
  if (!condition) {
    std::cerr << "FAIL: " << message << '\n';
    std::exit(1);
  }
}

void testFlowSensorTimeoutFallback() {
  FakeTime time;
  FakePulseCounter pulse_counter;
  spray::FlowSensor sensor(pulse_counter, time);
  sensor.begin();

  pulse_counter.setState(10U, 0U);
  time.setNowMs(spray::FLOW_STALE_TIMEOUT_MS + 10U);
  const float flow = sensor.readFlow();

  assertTrue(nearlyEqual(flow, spray::FLOW_STALE_FALLBACK_LPM), "flow timeout fallback");
  assertTrue(sensor.isStaleFaultActive(), "flow stale flag set");
}

void testFlowSensorClampToMax() {
  FakeTime time;
  FakePulseCounter pulse_counter;
  spray::FlowSensor sensor(pulse_counter, time);
  sensor.begin();

  time.setNowMs(1000U);
  pulse_counter.setState(100000U, 1000U);
  const float flow = sensor.readFlow();

  assertTrue(nearlyEqual(flow, spray::MAX_FLOW_LPM), "flow clamped to max");
  assertTrue(!sensor.isStaleFaultActive(), "flow stale flag clear");
}

void testWheelSensorTimeoutToZero() {
  FakeTime time;
  FakePulseCounter pulse_counter;
  spray::WheelSensor sensor(pulse_counter, time);
  sensor.begin();

  pulse_counter.setState(15U, 0U);
  time.setNowMs(spray::WHEEL_PULSE_TIMEOUT_MS + 1U);
  const float speed = sensor.readSpeed();

  assertTrue(nearlyEqual(speed, 0.0f), "wheel timeout to zero");
  assertTrue(sensor.isTimeoutFaultActive(), "wheel timeout flag set");
}

void testFlowControllerFallbackAndStop() {
  spray::FlowController controller;

  for (uint32_t i = 0; i < (spray::SENSOR_FAULT_TIMEOUT_MS / spray::LOOP_INTERVAL_MS) + 1U; ++i) {
    (void)controller.computePumpDuty(8.0f, 2.0f, 0.0f);
  }

  const uint8_t fallback = controller.computePumpDuty(8.0f, 2.0f, 0.0f);
  assertTrue(fallback == spray::SAFE_FALLBACK_DUTY, "controller fallback duty");

  uint8_t stopped = controller.computePumpDuty(0.0f, 2.0f, 5.0f);
  for (uint8_t i = 0; i < 8U && stopped != spray::PWM_MIN; ++i) {
    stopped = controller.computePumpDuty(0.0f, 2.0f, 5.0f);
  }
  assertTrue(stopped == spray::PWM_MIN, "controller stop semantics");
}

}  // namespace

int main() {
  testFlowSensorTimeoutFallback();
  testFlowSensorClampToMax();
  testWheelSensorTimeoutToZero();
  testFlowControllerFallbackAndStop();
  std::cout << "wave_04_unit_ok\n";
  return 0;
}
