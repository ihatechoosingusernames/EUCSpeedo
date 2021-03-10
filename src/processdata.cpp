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

double ProcessData::getDoubleData(DataType data_type) {
  xSemaphoreTake(data_mutex, portMAX_DELAY);  // Protecting the data array

  return data[(size_t)data_type];

  xSemaphoreGive(data_mutex);
}

}