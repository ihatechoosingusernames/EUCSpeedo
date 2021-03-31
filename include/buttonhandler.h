#ifndef BUTTONHANDLER_H_
#define BUTTONHANDLER_H_

#include <queue>

#include <Arduino.h>

#include "constants.h"

namespace euc {

// This class is a singleton in order to make ISR handling easier, also there is only one button ¯\_(ツ)_/¯
class ButtonHandler {
  public:
    static IRAM_ATTR ButtonHandler* getInstance();
    PressType getPress();

  private:
    static void IRAM_ATTR onTimer();

    ButtonHandler();

    static ButtonHandler* instance;
    hw_timer_t * timer;

    long int press_time, release_time;
    bool pressed = false;
    volatile bool press_complete = false;
    volatile PressType press_type = PressType::kNoPress;

    portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;
};

}

#endif