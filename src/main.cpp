/**
 * A wearable EUC Speedometer
 * author: Andrew Ellison
 */

#include <Arduino.h>
#include <configserver.h>

#include "eucspeedo.h"
#include "hwconfig.h"
#include "blehandler.h"

using namespace euc;

EucSpeedo* speedo_ptr;

void setup() {
  Serial.begin(115200);
  Serial.println("Starting EUC Speedo app");
  
  // speedo_ptr = new EucSpeedo();
  BleHandler ble([](EucType){}, [](uint8_t* data, size_t data_size){});
  ble.Scan();
}

void loop() {
  delay(100);
  // speedo_ptr->Process();
}

// TODO: Create more UI objects
// TODO: Fix bluetooth handler
// TODO: Implement config server functions to take text arguments
// TODO: Fix file saves
// TODO: Implement OTA updates
// TODO: Create settings object that loads stored global settings
// TODO: Implement controllers for Inmotion, Ninebot, and NinebotZ
// TODO: Gestures?
