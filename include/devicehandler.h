#ifndef DEVICEHANDLER_H_
#define DEVICEHANDLER_H_

#include <Arduino.h>
#include <pthread.h>
#include <mutex>
#include <list>

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

    void SetMpu9250Sleep();

    volatile uint32_t delay_time;  // 32 bits for max bus size
    long unsigned int timeout;

    std::array<double, 10> battery_array;
    size_t battery_array_pos = 0;
    double battery_array_sum = 0;

    pthread_t update_thread;
    std::mutex flash_mutex, battery_mutex;
    static portMUX_TYPE mux;
};

}

#endif