#include "processdata.h"
#include "logging.h"

namespace euc {

ProcessData::ProcessData() {}

void ProcessData::ApplySettings(Settings* settings_handler) {
  speed_factor = (settings_handler->getSetting(GeneralSetting::kDistanceUnits)? 1 : kMphConversionFactor);
  temp_in_freedoms = settings_handler->getSetting(GeneralSetting::kTemperatureUnits);
}

void ProcessData::Update(Euc* euc) {
  vTaskPrioritySet(NULL, 1);  // Set the updating task to a high priority to avoid deadlocks

  data_mutex.lock();  // Protecting the data array
    
  data[static_cast<size_t>(DataType::kSpeed)] = euc->getSpeed() * speed_factor;
  data[static_cast<size_t>(DataType::kVoltage)] = euc->getVoltage();
  data[static_cast<size_t>(DataType::kCurrent)] = euc->getCurrent();
  data[static_cast<size_t>(DataType::kPower)] = (euc->getCurrent() * euc->getVoltage()) / 100;
  data[static_cast<size_t>(DataType::kTemp)] = (temp_in_freedoms? (euc->getTemperature() * kFreedomsConversionFactor) + kFreedomsConversionOffset : euc->getTemperature());
  data[static_cast<size_t>(DataType::kBattery)] = euc->getBatteryPercent();
  data[static_cast<size_t>(DataType::kTripDistance)] = euc->getDistance()  * speed_factor;
  data[static_cast<size_t>(DataType::kTotalDistance)] = euc->getTotalDistance()  * speed_factor;

  data_mutex.unlock();
}

void ProcessData::Update(RtcHandler* rtc_handler, bool update_date) {
  rtc_handler->UpdateTime();

  data_mutex.lock();  // Protecting the data array

  data[static_cast<size_t>(DataType::kSecond)] = rtc_handler->getSecond();
  data[static_cast<size_t>(DataType::kMinute)] = rtc_handler->getMinute();
  data[static_cast<size_t>(DataType::kHour)] = rtc_handler->getHour();

  // Update date only the first second of every day, or if ordered to
  if (update_date || (!data[static_cast<size_t>(DataType::kHour)] && !data[static_cast<size_t>(DataType::kMinute)] && !data[static_cast<size_t>(DataType::kSecond)])) {
    rtc_handler->UpdateDate();
    data[static_cast<size_t>(DataType::kDay)] = rtc_handler->getDay();
    data[static_cast<size_t>(DataType::kMonth)] = rtc_handler->getMonth();
    data[static_cast<size_t>(DataType::kYear)] = rtc_handler->getYear();
  }

  data_mutex.unlock();
}

void ProcessData::Update(DataType type, double val) {
  data_mutex.lock();  // Protecting the data array

  data[static_cast<size_t>(type)] = val;

  data_mutex.unlock();
}

void ProcessData::Update(DeviceHandler* device) {
  data_mutex.lock();
  data[static_cast<size_t>(DataType::kWatchBattery)] = device->getBatteryPercentage();
  data_mutex.unlock();
}

double ProcessData::getDoubleData(DataType data_type) {
  return data[static_cast<size_t>(data_type)];
}

}