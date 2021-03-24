/**
 * A wearable EUC Speedometer
 * author: Andrew Ellison
 */

  /*
   * #include "esp_task_wdt.h"          // At beginning of code.
   * esp_task_wdt_reset();                // In your routine.
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

// TODO: Update BLE controller to nimBLE
// TODO: Fix Async server only serving half a page
// TODO: Create different screens for watch/EUC/Settings
// TODO: Create settings object that loads stored global settings
// TODO: Implement controllers for different EUC brands
// TODO: Gestures?