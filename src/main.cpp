/**
 * A wearable EUC Speedometer
 * author: Andrew Ellison
 */

#include <Arduino.h>
#include <configserver.h>

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

// TODO: Create different screens for watch/EUC/Settings
// TODO: Add buttons to change order of UI elements
// TODO: Make mock data functional
// TODO: Create settings object that loads stored global settings
// TODO: Implement controllers for different EUC brands
// TODO: Gestures?