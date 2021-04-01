#include "buttonhandler.h"

#include <Arduino.h>

#include "hwconfig.h"
#include "constants.h"

namespace euc {

ButtonHandler* ButtonHandler::instance = 0;

ButtonHandler* ButtonHandler::getInstance() {
  if (!instance) {
    instance = new ButtonHandler();
  }

  return instance;
}

PressType ButtonHandler::getPress() {
  if (press_complete) {
    PressType type = press_type;
    press_complete = false;
    press_type = PressType::kNoPress;
    return type;
  }

  return PressType::kNoPress;
}

ButtonHandler::ButtonHandler() {
  pinMode(PIN_223B_VDD, PULLUP);  // Pullup output mode allows the button to be used when MCU is in deep sleep mode
  pinMode(PIN_223B_Q, INPUT);  // By default input is low, active high

  digitalWrite(PIN_223B_VDD, HIGH);

  timer = timerBegin(0, BASE_CLOCK_HZ / 1000, true); // Setting up timer with prescaler for milliseconds, and counting up
  timerAttachInterrupt(timer, ButtonHandler::onTimer, true);
  timerAlarmWrite(timer, kMinPressTime / 2, true);

  timerAlarmEnable(timer);
}

void ButtonHandler::onTimer() {
  ButtonHandler* instance = getInstance();

  if (instance->press_complete)  // If the last press hasn't been handled, ignore future ones
    return;

  portENTER_CRITICAL_ISR(&instance->mux);
  if(digitalRead(PIN_223B_Q)) {
    if (!instance->pressed) {
      instance->press_time = millis();
      instance->pressed = true;

      if ((instance->press_time - instance->release_time < kMaxReleaseTime) && (instance->press_type == PressType::kSinglePress)) {
        instance->press_type = PressType::kDoublePress;
      } else {
        instance->press_type = PressType::kSinglePress;
      }

      // If press has been held long enough, trigger long press
    } else if ((millis() - instance->press_time > kMaxPressTime) && (instance->press_type != PressType::kNoPress)) {
      instance->press_type = PressType::kLongPress;
      instance->press_complete = true;
    }
  } else {
    if (instance->pressed) {
      instance->release_time = millis();
      instance->pressed = false;

      // If button has been either pressed or double pressed and has now been released, send it
    } else if ((millis() - instance->release_time > kMaxReleaseTime) && (instance->press_type != PressType::kNoPress)) {
      instance->press_complete = true; // No need for semaphores here, as booleans are atomic
    }
  }
  portEXIT_CRITICAL_ISR(&instance->mux);
}

}