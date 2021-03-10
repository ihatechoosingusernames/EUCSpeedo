#include "eucspeedo.h"

#include <functional>
#include <memory>

#include "gotway.h"
#include "constants.h"
#include "buttonhandler.h"

namespace euc {

EucSpeedo::EucSpeedo() : button_handler(ButtonHandler::getInstance()),
    ble(BleHandler(std::bind(&EucSpeedo::onFoundWheel, this, std::placeholders::_1),
    std::bind(&EucSpeedo::onProcessInput, this, std::placeholders::_1, std::placeholders::_2))) {
  button_handler->setCallback(std::bind(&EucSpeedo::onPress, this, std::placeholders::_1));
  ui_handler = UiHandler();
  process_data = ProcessData();
}

void EucSpeedo::Process() {
  process_data.Update(wheel.get());
  ui_handler.Update(&process_data);
}

// Creates the correct type of wheel object
void EucSpeedo::onFoundWheel(EucType type) {
  Serial.printf("Found %s EUC\n", kBrandName[(size_t)type]);

  if (wheel != nullptr) {
    wheel.reset();  // Remove an old instance if it still exists
  }

  switch (type) {
    case EucType::kGotway: {
      std::unique_ptr<Euc> wheel (new Gotway());
      wheel_created = true;
      break; }
    default:
      Serial.println("Wheel not yet implemented, uh oh!");
  }
}

void EucSpeedo::onProcessInput(uint8_t* data, size_t data_size) {
  if (wheel_created) {
    wheel->ProcessInput(data, data_size);  // Access the base implementation of ProcessInput()
  }
  // Update UI here
}

void EucSpeedo::onPress(PressType press_type) {
  Serial.print("Received press of type ");
  switch (press_type) {
    case PressType::kSinglePress:
      Serial.println("Single press");
      break;
    case PressType::kDoublePress:
      Serial.println("Double press");
      break;
    case PressType::kLongPress:
      Serial.println("Long press");
    default:
      break;
  }
}

}