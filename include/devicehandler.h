#ifndef DEVICEHANDLER_H_
#define DEVICEHANDLER_H_

#include <Arduino.h>

namespace euc {

class DeviceHandler {
  public:
    DeviceHandler();

    void Update();

    void FlashLed(uint32_t freq); // Frequency in Hz
    void LedOn();
    void LedOff();

    double getBatteryVoltage();
    double getBatteryPercentage();

  private:
    volatile uint32_t delay;  // 32 bits for max bus size
    long unsigned int timeout;
};

}

#endif