#ifndef EUC_H_
#define EUC_H_

#include <Arduino.h>

namespace euc {

class Euc {
  public:
    virtual ~Euc() = default;  // Declaring virtual destructor to allow safe "delete" calls on base class pointers
    
    virtual void ProcessInput(uint8_t data[], size_t data_len);

    double getSpeed() {return speed; };
    double getVoltage() { return voltage; };
    double getCurrent() { return current; };
    double getTemperature() { return temperature; };
    double getBatteryPercent() { return battery_percent; };
    double getDistance() { return distance; };
    double getTotalDistance() { return total_distance; };

  protected:
    double speed = 0, voltage = 0, current = 0, temperature = 0, temperature_2 = 0, battery_percent = 0, voltage_scaler = 0;
    long distance = 0, total_distance = 0;
};

}

#endif