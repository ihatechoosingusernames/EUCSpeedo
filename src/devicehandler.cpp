#include "devicehandler.h"

#include "hwconfig.h"
#include "constants.h"
#include "logging.h"

namespace euc {

portMUX_TYPE DeviceHandler::mux = portMUX_INITIALIZER_UNLOCKED;

DeviceHandler::DeviceHandler() {
  pinMode(PIN_BATT, INPUT);
  pinMode(PIN_CHARGE, INPUT);
  pinMode(PIN_LED, OUTPUT);

  attachInterrupt(PIN_CHARGE, onCharge, CHANGE);

  pthread_create(&update_thread, NULL, Update, (void*)this);
}

void* DeviceHandler::Update(void* in) {
  DeviceHandler* device = (DeviceHandler*)in;

  while(1) {
    device->flash_mutex.lock(); // Protects delay_time and timeout
    if (device->delay_time && device->timeout < millis()) {
      digitalWrite(PIN_LED, (digitalRead(PIN_LED)? LOW : HIGH));
      device->timeout += device->delay_time;
    }
    device->flash_mutex.unlock();

    device->battery_mutex.lock();
    // Last 10 battery voltages are stored and summed
    device->battery_array_sum -= device->battery_array[device->battery_array_pos];
    device->battery_array[device->battery_array_pos] = device->getBatteryVoltage();
    device->battery_array_sum += device->battery_array[device->battery_array_pos];

    device->battery_array_pos = ++(device->battery_array_pos) % device->battery_array.size();

    device->battery_mutex.unlock();

    delay(100);
  }
}

void DeviceHandler::FlashLed(uint32_t freq) {
  flash_mutex.lock(); // Protects delay_time and timeout

  delay_time = 1000 / freq;
  timeout = millis() + delay_time;

  flash_mutex.unlock();
}

void DeviceHandler::LedOn() {
  digitalWrite(PIN_LED, HIGH);
}

void DeviceHandler::LedOff() {
  flash_mutex.lock(); // Protects delay_time and timeout
  if (delay_time) {
    delay_time = 0;
  }
  flash_mutex.unlock();

  digitalWrite(PIN_LED, LOW);
}

double DeviceHandler::getBatteryVoltage() {
  double v = analogRead(PIN_BATT);
  return (v / 4095.0) * 2.0 * 3.3 * (kVoltageRef / 1000.0); // Taken directly from the Lilygo example code
}

double DeviceHandler::getBatteryPercentage() {
  battery_mutex.lock();
  double v = battery_array_sum / battery_array.size();  // Averaging the last 10 voltage values
  battery_mutex.unlock();

  if (v >= 3.7)
    return 100;
  else if (v > 3.0)
    return ((v - 3.0) / 0.7) * 100;
  else
    return 0;
}

void DeviceHandler::onCharge() {
  portENTER_CRITICAL_ISR(&mux);
  if (digitalRead(PIN_CHARGE) == LOW)
    pinMode(PIN_LED, PULLUP); // Ensures it stays on even in sleep
  else
    pinMode(PIN_LED, OUTPUT_OPEN_DRAIN);  // AKA off

  portEXIT_CRITICAL_ISR(&mux);
}

}