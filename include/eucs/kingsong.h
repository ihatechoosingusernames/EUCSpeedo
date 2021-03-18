#ifndef EUCS_KINGSONG_H_
#define EUCS_KINGSONG_H_

#include "euc.h"

namespace euc {

class Kingsong : public Euc {
  public:
    void ProcessInput(uint8_t data[], size_t data_len) override;
};

}

#endif