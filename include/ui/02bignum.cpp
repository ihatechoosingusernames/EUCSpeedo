#ifndef UI_BIGNUM_H_
#define UI_BIGNUM_H_

#include "uielement.h"

namespace ui {

class BigNum : public euc::UiElement {
  UI_SETUP(BigNum)
  
  public:
    BigNum(uint8_t data[], size_t data_len) {
      size = 1;
    };

    void Draw() override {
      printf("BigNum::Draw()\n");
    };

    size_t DataSize() override {
      return size;
    };

  private:
    size_t size;
};

UI_REGISTER(BigNum, 0x02)

}

#endif