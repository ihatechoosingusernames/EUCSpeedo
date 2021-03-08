#ifndef EUCSPEEDO_H_
#define EUCSPEEDO_H_

#include "euc.h"
#include "blehandler.h"
#include "constants.h"
#include "buttonhandler.h"

namespace euc {

class EucSpeedo {
  public:
    EucSpeedo();
    ~EucSpeedo();

    void Process(); // To be called periodically

  private:
    void onFoundWheel(EucType type);
    void onProcessInput(uint8_t* data, size_t data_size);
    void onPress(PressType press_type);

    Euc* wheel; // Maybe I should use smart pointers
    ButtonHandler* button_handler;
    BleHandler ble;
    // Display and UI adapter will go here

    bool wheel_created = false;
};

}

#endif