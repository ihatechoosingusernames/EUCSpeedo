#ifndef BUTTONHANDLER_H_
#define BUTTONHANDLER_H_

#include <functional>

#include <Arduino.h>

#include "constants.h"

namespace euc {

// This class is a singleton in order to make ISR handling easier, also there is only one button ¯\_(ツ)_/¯
class ButtonHandler {
  public:
    static void setCallback(std::function<void(PressType type)> press_callback);

  private:
    ButtonHandler();

    static void IRAM_ATTR onPress();
    static void IRAM_ATTR onRelease();
    static void IRAM_ATTR onTimer();

    static std::function<void(PressType type)> IRAM_ATTR callback;
    static hw_timer_t * timer;

    static volatile long int press_time, release_time;
    static volatile bool pressed;
    static PressType press_type;

    static portMUX_TYPE mux;
};

}

#endif