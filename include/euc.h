#ifndef EUC_H_
#define EUC_H_

#include <Arduino.h>

namespace euc {

class Euc {
  public:
    virtual ~Euc() = default;  // Declaring virtual destructor to allow safe "delete" calls on base class pointers
    
    virtual void ProcessInput(uint8_t data[], size_t data_len);

    double getSpeed() {return speed / 100.0; };

  protected:
    int voltage, current, temperature, temperature_2, battery_percent, voltage_scaler;
    long distance, total_distance;
    double speed;
};

}

#endif