#include "devicehandler.h"

#include <Wire.h>

#include "hwconfig.h"
#include "constants.h"
#include "logging.h"

namespace euc {

portMUX_TYPE DeviceHandler::mux = portMUX_INITIALIZER_UNLOCKED;

DeviceHandler::DeviceHandler() {
  pinMode(PIN_BATT, INPUT);
  pinMode(PIN_CHARGE, INPUT);
  pinMode(PIN_LED, OUTPUT);

  SetMpu9250Sleep();

  attachInterrupt(PIN_CHARGE, onCharge, CHANGE);

  double volts = getBatteryVoltage(); // Start off the whole array at the current voltage
  for (size_t index = 0; index < battery_array.size(); index++)
    battery_array[index] = volts;

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

void DeviceHandler::SetMpu9250Sleep() {
  // Reading the PWR_MGMT register from the MPU9250
  Wire.beginTransmission(kMPU9250_ADDRESS);
  Wire.write(kPWR_MGMT_1);
  Wire.endTransmission(false);
  Wire.requestFrom(kMPU9250_ADDRESS, (uint8_t) 1);
  uint8_t b = Wire.read();

  // Copying the new bit into the PWR_MGMT byte
  b = b | (1 << kPWR1_SLEEP_BIT);

  // Writing the updated byte back into the register
  Wire.beginTransmission(kMPU9250_ADDRESS);
  Wire.write(kPWR_MGMT_1);
  Wire.write(b);
  Wire.endTransmission();
}

}