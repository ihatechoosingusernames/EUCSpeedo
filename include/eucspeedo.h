#ifndef EUCSPEEDO_H_
#define EUCSPEEDO_H_

#include <memory>

#include "euc.h"
#include "blehandler.h"
#include "constants.h"
#include "buttonhandler.h"
#include "processdata.h"
#include "uihandler.h"

namespace euc {

class EucSpeedo {
  public:
    EucSpeedo();

    void Process(); // To be called periodically

  private:
    void onFoundWheel(EucType type);
    void onProcessInput(uint8_t* data, size_t data_size);
    void onPress(PressType press_type);

    std::unique_ptr<Euc> wheel;
    ButtonHandler* button_handler;
    BleHandler ble;
    UiHandler ui_handler;
    ProcessData process_data;

    bool wheel_created = false;
};

}

#endif