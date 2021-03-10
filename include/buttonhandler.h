#ifndef BUTTONHANDLER_H_
#define BUTTONHANDLER_H_

#include <functional>
#include <queue>

#include <Arduino.h>

#include "constants.h"

namespace euc {

// This class is a singleton in order to make ISR handling easier, also there is only one button ¯\_(ツ)_/¯
class ButtonHandler {
  public:
    static ButtonHandler* getInstance();

    void setCallback(std::function<void(PressType type)> press_callback);
    void Process();

  private:
    ButtonHandler();

    static void IRAM_ATTR onTimer();

    static ButtonHandler* instance;

    std::function<void(PressType type)> callback;
    hw_timer_t * timer;

    long int press_time, release_time;
    bool callback_set = false, pressed = false;
    PressType press_type = PressType::kNoPress;

    std::queue<PressType> press_queue;

    portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;
    SemaphoreHandle_t queue_mutex;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;  // Necessary for ISR semaphores
};

}

#endif