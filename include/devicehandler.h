#ifndef DEVICEHANDLER_H_
#define DEVICEHANDLER_H_

#include <Arduino.h>
#include <pthread.h>
#include <mutex>

namespace euc {

class DeviceHandler {
  public:
    DeviceHandler();

    void FlashLed(uint32_t freq); // Frequency in Hz
    void LedOn();
    void LedOff();

    double getBatteryVoltage();
    double getBatteryPercentage();

  private:
    static void* Update(void* in);
    static void IRAM_ATTR onCharge();

    volatile uint32_t delay_time;  // 32 bits for max bus size
    long unsigned int timeout;

    pthread_t update_thread;
    std::mutex flash_mutex;
};

}

#endif