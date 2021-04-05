/**
 * A wearable EUC Speedometer
 * author: Andrew Ellison
 */

#include <Arduino.h>

#include "eucspeedo.h"
#include "logging.h"

using namespace euc;

EucSpeedo* speedo_ptr;

void setup() {
  Serial.begin(115200);
  LOG_DEBUG("Starting EUC Speedo app")
  
  speedo_ptr = new EucSpeedo();
}

void loop() {
  delay(100);
  speedo_ptr->Process();
}

// TODO: Create more UI objects
// TODO: Update bluetooth handler to gracefully deal with reconnects without using a ton of memory
// TODO: Implement OTA updates
// TODO: Create settings object that loads stored global settings
// TODO: Implement controllers for Inmotion, Ninebot, and NinebotZ
// TODO: Gestures?
