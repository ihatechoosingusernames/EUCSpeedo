#ifndef PROCESSDATA_H_
#define PROCESSDATA_H_

#include "euc.h"
#include "constants.h"
#include "rtchandler.h"

namespace euc {

  /*
    This class provides the UI elements access to all available device data.
    It is thread safe, for both updates and accesses.
  */
class ProcessData {
  public:
    ProcessData();

    void Update(Euc* euc);
    void Update(RtcHandler* rtc, bool update_date = false);

    double getDoubleData(DataType data_type);

  private:
    double data[(size_t)DataType::kLastValue] = {0.0};  // Of this size to fit all DataTypes

    SemaphoreHandle_t data_mutex; // Semaphore to protect the data array
};

}

#endif