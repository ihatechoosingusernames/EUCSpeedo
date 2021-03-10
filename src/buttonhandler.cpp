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

void ButtonHandler::setCallback(std::function<void(PressType type)> press_callback) {
  callback = press_callback;

  // Now we have a callback we can use the interrupt
  timerAttachInterrupt(timer, ButtonHandler::onTimer, true);
  timerAlarmWrite(instance->timer, kMinPressTime / 2, true);
  timerAlarmEnable(instance->timer);

  callback_set = true;
}

void ButtonHandler::Process() {
  if (xSemaphoreTake(queue_mutex, portMAX_DELAY) == pdPASS) {
    while (!press_queue.empty()) {
      callback(press_queue.front());
      press_queue.pop();
    }
    xSemaphoreGive(instance->queue_mutex);
  }
}

ButtonHandler::ButtonHandler() {
  pinMode(PIN_223B_VDD, OUTPUT);
  pinMode(PIN_223B_Q, INPUT_PULLUP);  // By default input is pulled low, active high

  timer = timerBegin(0, BASE_CLOCK_HZ / 1000, true); // Setting up timer with prescaler for milliseconds, and counting up

  digitalWrite(PIN_223B_VDD, HIGH); // Powers on 223B touch button

  queue_mutex = xSemaphoreCreateBinary(); // Creates semaphore to protect press queue
}

void ButtonHandler::onTimer() {
  ButtonHandler* instance = getInstance();

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
      xSemaphoreTakeFromISR(instance->queue_mutex, &instance->xHigherPriorityTaskWoken);
      instance->press_queue.push(PressType::kLongPress);
      xSemaphoreGiveFromISR(instance->queue_mutex, &instance->xHigherPriorityTaskWoken);
      instance->press_type = PressType::kNoPress;
    }
  } else {
    if (instance->pressed) {
      instance->release_time = millis();
      instance->pressed = false;

      // If button has been either pressed or double pressed and has now been released, send it
    } else if ((millis() - instance->release_time > kMaxReleaseTime) && (instance->press_type != PressType::kNoPress)) {
      xSemaphoreTakeFromISR(instance->queue_mutex, &instance->xHigherPriorityTaskWoken);
      instance->press_queue.push(instance->press_type);
      xSemaphoreGiveFromISR(instance->queue_mutex, &instance->xHigherPriorityTaskWoken);
      instance->press_type = PressType::kNoPress;
    }
  }
  portEXIT_CRITICAL_ISR(&instance->mux);
}

}