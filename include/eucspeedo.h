#ifndef EUCSPEEDO_H_
#define EUCSPEEDO_H_

#include <queue>

#include "euc.h"
#include "blehandler.h"
#include "constants.h"
#include "buttonhandler.h"
#include "processdata.h"
#include "uihandler.h"
#include "filehandler.h"
#include "configserver.h"
#include "rtchandler.h"

namespace euc {

class EucSpeedo {
  public:
    EucSpeedo();
    ~EucSpeedo();

    void Process(); // To be called periodically

  private:
    void onFoundWheel(EucType type);
    void onProcessInput(uint8_t* data, size_t data_size);
    void HandlePress(PressType press);

    Euc* wheel;
    ButtonHandler* button_handler;
    BleHandler* ble;
    ConfigServer* config_server;
    FileHandler file_handler;
    UiHandler ui_handler;
    ProcessData process_data;
    RtcHandler rtc_handler;

    bool wheel_connected = false, config_server_active = false, ble_handler_active = false;
};

}

#endif