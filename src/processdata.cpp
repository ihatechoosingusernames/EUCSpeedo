#include "processdata.h"

namespace euc {

ProcessData::ProcessData() {
}

void ProcessData::Update(Euc* euc) {
  if(!writing) {  // Protecting the data array
    writing = true;

    data[static_cast<size_t>(DataType::kSpeed)] = euc->getSpeed();
    data[static_cast<size_t>(DataType::kVoltage)] = euc->getVoltage();
    data[static_cast<size_t>(DataType::kCurrent)] = euc->getCurrent();
    data[static_cast<size_t>(DataType::kTemp)] = euc->getTemperature();
    data[static_cast<size_t>(DataType::kBattery)] = euc->getBatteryPercent();
    data[static_cast<size_t>(DataType::kTripDistance)] = euc->getDistance();
    data[static_cast<size_t>(DataType::kTotalDistance)] = euc->getTotalDistance();

    writing = false;

  } else {
    printf("ProcessData::getDoubleData Could not get access to the data_mutex\n");
  }
}

void ProcessData::Update(RtcHandler* rtc_handler, bool update_date) {
  rtc_handler->UpdateTime();

  if(!writing) {  // Protecting the data array
    writing = true;

    data[static_cast<size_t>(DataType::kSecond)] = rtc_handler->getSecond();
    data[static_cast<size_t>(DataType::kMinute)] = rtc_handler->getMinute();
    data[static_cast<size_t>(DataType::kHour)] = rtc_handler->getHour();

    if (update_date || (data[static_cast<size_t>(DataType::kHour)] == 0 && data[static_cast<size_t>(DataType::kMinute)] == 0)) { // Update date only the first minute of every day, or if ordered to
      rtc_handler->UpdateDate();
      data[static_cast<size_t>(DataType::kDay)] = rtc_handler->getDay();
      data[static_cast<size_t>(DataType::kMonth)] = rtc_handler->getMonth();
      data[static_cast<size_t>(DataType::kYear)] = rtc_handler->getYear();
    }

    writing = false;

  } else {
    printf("ProcessData::getDoubleData Could not get access to the data_mutex\n");
  }
}

void ProcessData::Update(DataType type, double val) {
  if(!writing) {  // Protecting the data array
    writing = true;

    data[static_cast<size_t>(type)] = val;

    writing = false;
  } else {
    printf("ProcessData::Update Could not get access to the data_mutex\n");
  }
}

double ProcessData::getDoubleData(DataType data_type) {
  if(!writing) {  // Protecting the data array

    return data[static_cast<size_t>(data_type)];

  } else {
    printf("ProcessData::getDoubleData Could not get access to the data_mutex\n");
    return 0;
  }
}

}