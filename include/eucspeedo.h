#ifndef EUCSPEEDO_H_
#define EUCSPEEDO_H_

#include "euc.h"
#include "blehandler.h"
#include "constants.h"

namespace euc {

class EucSpeedo {
  public:
    EucSpeedo();
    ~EucSpeedo();

  private:
    void onFoundWheel(EucType type);
    void onProcessInput(uint8_t* data, size_t data_size);

    Euc* wheel; // Maybe I should use smart pointers
    BleHandler ble;
    // Display and UI adapter will go here

    bool wheel_created = false;
};

}

#endif