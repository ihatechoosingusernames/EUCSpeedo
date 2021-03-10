/**
 * A wearable EUC Speedometer
 * author: Andrew Ellison
 */

#include <Arduino.h>
#include <BLEDevice.h>

#include "eucspeedo.h"

using namespace euc;

EucSpeedo* speedo_ptr;

void setup() {
  Serial.begin(115200);
  Serial.println("Starting EUC Speedo app");
  
  speedo_ptr = new EucSpeedo();
}

void loop() {
  delay(100);
  speedo_ptr->Process();
}
