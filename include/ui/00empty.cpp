#ifndef UI_EMPTY_CPP_
#define UI_EMPTY_CPP_

#include "uielement.h"

namespace ui {

class Empty : public euc::UiElement {
  UI_SETUP(Empty)
  public:
    Empty(uint8_t data[], size_t data_len) {}

    void Draw() override {
      printf("Empty::Draw()\n");
    };

    size_t DataSize() override {
      return 1; // Even the empty element uses 1 byte for its name
    };
};

UI_REGISTER(Empty, 0x00)

}

#endif