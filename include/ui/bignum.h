#ifndef BIGNUM_H_
#define BIGNUM_H_

#include "uielement.h"

namespace ui {

class BigNum : public euc::UiElement {
  public:
    BigNum() {
      return;
    };

    void Draw() override {
      return;
    };

    size_t DataSize() override {
      return size;
    };

  private:
    size_t size;
};

}

#endif