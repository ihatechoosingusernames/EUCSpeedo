#include "eucspeedo.h"

#include <functional>

#include "gotway.h"
#include "constants.h"

namespace euc {

EucSpeedo::EucSpeedo() : ble(BleHandler(
    std::bind(&EucSpeedo::onFoundWheel, this, std::placeholders::_1),
    std::bind(&EucSpeedo::onProcessInput, this, std::placeholders::_1, std::placeholders::_2))) {}

EucSpeedo::~EucSpeedo() {
  delete wheel;
}

// Creates the correct type of wheel object
void EucSpeedo::onFoundWheel(EucType type) {
  printf("Found %s EUC\n", kBrandName[(size_t)type]);

  switch (type) {
    case EucType::kGotway:
      *wheel = Gotway();
      wheel_created = true;
      break;
    default:
      printf("Wheel not yet implemented, uh oh!\n");
  }
}

void EucSpeedo::onProcessInput(uint8_t* data, size_t data_size) {
  if (wheel_created) {
    wheel->ProcessInput(data, data_size);
  }
  // Update UI here
}

}