#ifndef GOTWAY_H_
#define GOTWAY_H_

#include <Arduino.h>

#include "euc.h"
namespace euc {

class Gotway : public Euc {
  public:
    void ProcessInput(uint8_t data[], size_t data_len) override;

    double getSpeed() override;

    bool isVeteran();

  private:
    bool is_veteran;
    
    const double ratio = 0.875; // Gotway Magic number
};

}

#endif