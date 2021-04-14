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
  #ifdef DEBUG  // Only bother with serial if we're using debug logging
  Serial.begin(115200);
  #endif

  LOG_DEBUG("Starting EUC Speedo app")
  
  speedo_ptr = new EucSpeedo();
}

void loop() {
  delay(100);
  speedo_ptr->Process();
}

// TODO: Fix processdata having no mutexes
// TODO: Add OTA file system image update
// TODO: Add portrait/landscape screen option
// TODO: Update bluetooth handler to gracefully deal with reconnects without using a ton of memory
// TODO: Implement controllers for Inmotion, Ninebot, and NinebotZ
// TODO: Gestures?
