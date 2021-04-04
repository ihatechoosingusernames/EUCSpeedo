#include "eucspeedo.h"

#include <functional>
#include <memory>

#include "eucs/includes.h"
#include "constants.h"
#include "buttonhandler.h"

namespace euc {

EucSpeedo::EucSpeedo() : button_handler(ButtonHandler::getInstance()),
    file_handler(),
    ui_handler(&file_handler),
    process_data() {
  file_handler.listDir("/", 1);

  process_data.Update(&rtc_handler, true);  // Check the date on first start
}

EucSpeedo::~EucSpeedo() {
  delete wheel; // Clean up wheel pointer
  if (config_server_active) {
    config_server->Stop();
    delete config_server;
  }
  if (ble_handler_active)
    delete ble;
}

void EucSpeedo::Process() {
  HandlePress(button_handler->getPress());
  process_data.Update(&rtc_handler);
  if (!config_server_active)  // Config server is asynchronous and takes over control of the UI
    ui_handler.Update(&process_data);

  if (ble_handler_active)
    ble->Update();
}

// Creates the correct type of wheel object
void EucSpeedo::onFoundWheel(EucType type) {
  Serial.printf("Found %s EUC\n", kBrandName[(size_t)type]);

  switch (type) {
    case EucType::kGotway:
      wheel = new Gotway();
      wheel_connected = true;
      break;
    case EucType::kKingsong:
      wheel = new Kingsong();
      wheel_connected = true;
      break;
    default:
      Serial.println("Wheel not yet implemented, uh oh!");
  }
}

void EucSpeedo::onProcessInput(uint8_t* data, size_t data_size) {
  if (wheel_connected) {
    wheel->ProcessInput(data, data_size);  // Let the specific wheel process the data
    process_data.Update(wheel); // Update the wheel data supplied to the UI
  }
}

void EucSpeedo::HandlePress(PressType press) {
  if (config_server_active && press != PressType::kLongPress) // This is hacky, should all be replaced with a state machine
    return;

  switch (press) {
    case PressType::kSinglePress:
      Serial.println("Single press\n");
      if (static_cast<uint8_t>(ui_handler.getCurrentScreen()) >= static_cast<uint8_t>(UiScreen::kCustom)) {
        ui_handler.Sleep();
      } else {
        ui_handler.ChangeScreen(static_cast<UiScreen>(static_cast<uint8_t>(ui_handler.getCurrentScreen()) + 1));
        ui_handler.Update(&process_data);
      }
      break;
    case PressType::kDoublePress:
      Serial.println("Double press\n");
      if (ble_handler_active) {
        delete ble;
        ble_handler_active = false;
      } else {
        ble = new BleHandler(std::bind(&EucSpeedo::onFoundWheel, this, std::placeholders::_1),
          std::bind(&EucSpeedo::onProcessInput, this, std::placeholders::_1, std::placeholders::_2));
        ble_handler_active = true;
        ble->Scan();
      }
      break;
    case PressType::kLongPress:
      Serial.println("Long press\n");
      if (config_server_active) {
        config_server->Stop();
        delete config_server;
        config_server_active = false;
      } else {
        config_server = new ConfigServer(&ui_handler, &file_handler, &rtc_handler);
        config_server->Start();
        config_server_active = true;
      }
    default:
      break;
  }
}

}