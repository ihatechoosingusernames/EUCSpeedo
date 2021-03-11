#ifndef UI_00EMPTY_CPP_
#define UI_00EMPTY_CPP_

#include "uielement.h"

namespace euc {

class Empty : public UiElement {
  UI_SETUP(Empty)
  public:
    Empty(uint8_t data[], size_t data_len) {}

    void Draw(ProcessData* data) {
      printf("Empty::Draw()\n");
    }
};

UI_REGISTER(Empty, 0x00)

}

#endif