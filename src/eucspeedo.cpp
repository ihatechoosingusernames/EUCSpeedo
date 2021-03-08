#include "eucspeedo.h"

#include <functional>

#include "gotway.h"
#include "constants.h"
#include "buttonhandler.h"

namespace euc {

EucSpeedo::EucSpeedo() : button_handler(ButtonHandler::getInstance()),
    ble(BleHandler(std::bind(&EucSpeedo::onFoundWheel, this, std::placeholders::_1),
    std::bind(&EucSpeedo::onProcessInput, this, std::placeholders::_1, std::placeholders::_2))) {
  button_handler->setCallback(std::bind(&EucSpeedo::onPress, this, std::placeholders::_1));
}

EucSpeedo::~EucSpeedo() {
  delete wheel;
}

void EucSpeedo::Process() {
  // button_handler->Process();
}

// Creates the correct type of wheel object
void EucSpeedo::onFoundWheel(EucType type) {
  Serial.printf("Found %s EUC\n", kBrandName[(size_t)type]);

  switch (type) {
    case EucType::kGotway:
      wheel = new Gotway();
      wheel_created = true;
      break;
    default:
      Serial.println("Wheel not yet implemented, uh oh!");
  }
}

void EucSpeedo::onProcessInput(uint8_t* data, size_t data_size) {
  if (wheel_created) {
    wheel->ProcessInput(data, data_size);
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