#include "processdata.h"

namespace euc {

ProcessData::ProcessData() {}

void ProcessData::Update(Euc* euc) {
  data[(size_t)DataType::kSpeed] = euc->getSpeed();
}

double ProcessData::getDoubleData(DataType data_type) {
  return data[(size_t)data_type];
}

}