#ifndef EUC_INMOTION_H_
#define EUC_INMOTION_H_

#include "euc.h"

namespace euc {

class Inmotion : public Euc {
  public:
    void ProcessInput(uint8_t data[], size_t data_len) override;
};

}

#endif