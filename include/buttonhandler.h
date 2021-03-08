#ifndef BUTTONHANDLER_H_
#define BUTTONHANDLER_H_

#include <functional>

#include <Arduino.h>

#include "constants.h"

namespace euc {

// This class is a singleton in order to make ISR handling easier, also there is only one button ¯\_(ツ)_/¯
class ButtonHandler {
  public:
    static ButtonHandler* getInstance();

    void setCallback(std::function<void(PressType type)> press_callback);
    void Process(); // To be called periodically

  private:
    ButtonHandler();

    static void IRAM_ATTR onTimer();

    static ButtonHandler* instance;

    std::function<void(PressType type)> callback;
    hw_timer_t * timer;

    volatile long int press_time, release_time;
    volatile bool callback_set = false, pressed = false;
    volatile PressType press_type = PressType::kNoPress;

    portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;
};

}

#endif