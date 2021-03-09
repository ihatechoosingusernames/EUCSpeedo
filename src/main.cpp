/**
 * First pass at a Gotway BLE client, working from the WheelLog project at https://github.com/palachzzz/WheelLogAndroid.
 * author Andrew Ellison
 */
#include <Arduino.h>
#include <BLEDevice.h>

#include "eucspeedo.h"
#include "uielement.h"

using namespace euc;

EucSpeedo* speedo_ptr;

void setup() {
  Serial.begin(115200);
  Serial.println("Starting EUC Speedo app");

  uint8_t data[1] = {0x02};
  UiElement* elem = UiElement::Factory(data, 1);
  elem->Draw();
  
  // speedo_ptr = new EucSpeedo();
}

void loop() {
  delay(100);
  // speedo_ptr->Process();
}
