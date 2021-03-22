#ifndef EUCS_KINGSONG_H_
#define EUCS_KINGSONG_H_

#include "euc.h"

namespace euc {

class Kingsong : public Euc {
  public:
    void ProcessInput(uint8_t data[], size_t data_len) override;

  private:
    String model;

    uint8_t mode, max_speed, alarm_1_speed, alarm_2_speed, alarm_3_speed;

    static constexpr double ks18l_scaler = 0.83; // Kingsong magic number
};

}

#endif