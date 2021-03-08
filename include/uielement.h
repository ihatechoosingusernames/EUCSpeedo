#ifndef UIELEMENT_H_
#define UIELEMENT_H_

#include <Arduino.h>

namespace euc {

class UiElement {
  public:
    static UiElement Factory(uint8_t data[], size_t data_len);

    virtual void Draw();
    virtual size_t DataSize();
};

}

#endif