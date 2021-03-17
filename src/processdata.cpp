#include "processdata.h"

namespace euc {

ProcessData::ProcessData() {
  data_mutex = xSemaphoreCreateMutex();
}

void ProcessData::Update(Euc* euc) {
  xSemaphoreTake(data_mutex, portMAX_DELAY);  // Protecting the data array

  data[(size_t)DataType::kSpeed] = euc->getSpeed();

  xSemaphoreGive(data_mutex);
}

void ProcessData::Update(RtcHandler* rtc_handler, bool update_date) {
  rtc_handler->UpdateTime();

  xSemaphoreTake(data_mutex, portMAX_DELAY);  // Protecting the data array

  data[(size_t)DataType::kSecond] = rtc_handler->getSecond();
  data[(size_t)DataType::kMinute] = rtc_handler->getMinute();
  data[(size_t)DataType::kHour] = rtc_handler->getHour();

  if (update_date || (data[(size_t)DataType::kHour] == 0 && data[(size_t)DataType::kMinute] == 0)) { // Update date only the first minute of every day, or if ordered to
    rtc_handler->UpdateDate();
    data[(size_t)DataType::kDay] = rtc_handler->getDay();
    data[(size_t)DataType::kMonth] = rtc_handler->getMonth();
    data[(size_t)DataType::kYear] = rtc_handler->getYear();
  }

  xSemaphoreGive(data_mutex);
}

double ProcessData::getDoubleData(DataType data_type) {
  xSemaphoreTake(data_mutex, portMAX_DELAY);  // Protecting the data array

  return data[(size_t)data_type];

  xSemaphoreGive(data_mutex);
}

}