#include "eucspeedo.h"

#include <functional>
#include <memory>

#include "eucs/includes.h"
#include "constants.h"
#include "buttonhandler.h"
#include "hwconfig.h"

namespace euc {

EucSpeedo::EucSpeedo() : button_handler(ButtonHandler::getInstance()),
    ble(BleHandler(std::bind(&EucSpeedo::onFoundWheel, this, std::placeholders::_1), std::bind(&EucSpeedo::onProcessInput, this, std::placeholders::_1, std::placeholders::_2))),
    file_handler(),
    ui_handler(&file_handler),
    process_data(),
    config_server(&ui_handler, &file_handler) {
  file_handler.listDir("/", 1);
  // ble.Scan();

  process_data.Update(&rtc_handler, true);  // Check the date on first start
}

EucSpeedo::~EucSpeedo() {
  delete wheel; // Clean up wheel pointer
}

void EucSpeedo::Process() {
  HandlePress(button_handler->getPress());
  process_data.Update(&rtc_handler);
  if (!config_server.isStarted())  // Config server is asynchronous and takes over control of the UI
    ui_handler.Update(&process_data);
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
  switch (press) {
    case PressType::kSinglePress:
      Serial.println("Single press\n");
      break;
    case PressType::kDoublePress:
      Serial.println("Double press\n");
      break;
    case PressType::kLongPress:
      Serial.println("Long press\n");
      if (config_server.isStarted()) {
        config_server.Stop();
      } else {
        config_server.Start();
      }
    default:
      break;
  }
}

}