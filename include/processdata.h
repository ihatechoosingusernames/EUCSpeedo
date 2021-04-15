#ifndef PROCESSDATA_H_
#define PROCESSDATA_H_

#include "euc.h"
#include "constants.h"
#include "rtchandler.h"
#include "settings.h"
#include "devicehandler.h"
#include <mutex>

namespace euc {

  /*
    This class provides the UI elements access to all available device data.
    It is thread safe, for both updates and accesses.
  */
class ProcessData {
  public:
    ProcessData();

    void ApplySettings(Settings* settings_handler);

    void Update(Euc* euc);
    void Update(RtcHandler* rtc, bool update_date = false);
    void Update(DeviceHandler* device);
    void Update(DataType type, double val);

    double getDoubleData(DataType data_type);

  private:
    double data[static_cast<size_t>(DataType::kLastValue)] = {{0.0}};  // Of this size to fit all DataTypes
    
    std::recursive_mutex data_mutex;

    double speed_factor = 1.0;
    bool temp_in_freedoms = false;
};

}

#endif