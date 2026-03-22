// Minimal compile-safe sketch for CI.
// Target board: arduino:avr:nano

namespace spray_controller {

constexpr uint8_t PIN_WHEEL_SENSOR = 2;
constexpr uint8_t PIN_FLOW_SENSOR = 3;
constexpr uint8_t PIN_PUMP_PWM = 11;
constexpr uint8_t PIN_SECTION_1 = 12;
constexpr uint8_t PIN_SECTION_2 = 13;
constexpr uint8_t PIN_SECTION_3 = A1;
constexpr uint8_t PIN_LED_SECTION_1 = A2;
constexpr uint8_t PIN_RUN_HOLD = 4;

constexpr uint8_t PWM_OFF = 0;

void configurePins() {
  pinMode(PIN_WHEEL_SENSOR, INPUT_PULLUP);
  pinMode(PIN_FLOW_SENSOR, INPUT_PULLUP);

  pinMode(PIN_SECTION_1, OUTPUT);
  pinMode(PIN_SECTION_2, OUTPUT);
  pinMode(PIN_SECTION_3, OUTPUT);
  pinMode(PIN_LED_SECTION_1, OUTPUT);
  pinMode(PIN_PUMP_PWM, OUTPUT);

  pinMode(PIN_RUN_HOLD, INPUT_PULLUP);
}

void setSafeOutputs() {
  digitalWrite(PIN_SECTION_1, LOW);
  digitalWrite(PIN_SECTION_2, LOW);
  digitalWrite(PIN_SECTION_3, LOW);
  digitalWrite(PIN_LED_SECTION_1, LOW);
  analogWrite(PIN_PUMP_PWM, PWM_OFF);
}

void updateRunHoldState() {
  const bool isRun = digitalRead(PIN_RUN_HOLD) == LOW;
  if (isRun) {
    digitalWrite(PIN_LED_SECTION_1, HIGH);
  } else {
    digitalWrite(PIN_LED_SECTION_1, LOW);
    analogWrite(PIN_PUMP_PWM, PWM_OFF);
  }
}

}  // namespace spray_controller

void setup() {
  Serial.begin(115200);
  spray_controller::configurePins();
  spray_controller::setSafeOutputs();
}

void loop() {
  spray_controller::updateRunHoldState();
  delay(10);
}
