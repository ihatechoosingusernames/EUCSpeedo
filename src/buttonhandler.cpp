#include "buttonhandler.h"

#include <Arduino.h>

#include "hwconfig.h"
#include "constants.h"

namespace euc {

portMUX_TYPE ButtonHandler::mux = portMUX_INITIALIZER_UNLOCKED;
PressType ButtonHandler::press_type = PressType::kNoPress;

void ButtonHandler::setCallback(std::function<void(PressType type)> press_callback) {
  callback = press_callback;
}

ButtonHandler::ButtonHandler() {
  pinMode(PIN_223B_VDD, OUTPUT);
  pinMode(PIN_223B_Q, INPUT);  // By default input is pulled low

  attachInterrupt(PIN_223B_Q, ButtonHandler::onPress, RISING); // By default input is active high
  attachInterrupt(PIN_223B_Q, ButtonHandler::onRelease, FALLING);

  timer = timerBegin(0, BASE_CLOCK_HZ / 1000, true); // Setting up timer with prescaler for milliseconds, and counting up

  timerAttachInterrupt(timer, ButtonHandler::onTimer, true);

  digitalWrite(PIN_223B_VDD, HIGH); // Powers on 223B touch button
}

void ButtonHandler::onPress() {
  portENTER_CRITICAL_ISR(&mux);
  press_time = millis();
  pressed = true;
  portEXIT_CRITICAL_ISR(&mux);
}

void ButtonHandler::onRelease() {
  portENTER_CRITICAL_ISR(&mux);
  release_time = millis();
  pressed = false;
  portEXIT_CRITICAL_ISR(&mux);

  // Debouncing
  long hold_time = release_time - press_time;

  if (hold_time > kMaxPressTime) {
    callback(PressType::kLongPress);
    press_type = PressType::kNoPress;
  } else if (press_type == PressType::kSinglePress && hold_time > kMinPressTime) {
    callback(press_type);
    press_type = PressType::kNoPress;
  } else if (hold_time > kMinPressTime) {
    press_type = PressType::kSinglePress;
    timerAlarmWrite(timer, kMaxReleaseTime, false); // Start Countdown timer for release
    timerAlarmEnable(timer);
  }
}

void ButtonHandler::onTimer() {
  portENTER_CRITICAL_ISR(&mux);
  if (!pressed) {
    callback(press_type);
    press_type = PressType::kNoPress;
  }
  portEXIT_CRITICAL_ISR(&mux);
}

}