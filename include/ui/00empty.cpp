#ifndef UI_00EMPTY_CPP_
#define UI_00EMPTY_CPP_

#include "uielement.h"

namespace euc {

class Empty : public UiElement {
  public:
    // Empty specifically does not use the UI_SETUP macro as it needs to accept data element codes that do not match it.
    static UiElement* Builder(uint8_t data[], size_t data_len) {
      return (UiElement*) new Empty(data, data_len);
    };
    ~Empty() {};
    std::vector<ArgType> ArgList() { return {}; }
    String Name() { return "Empty"; }
    Empty(uint8_t data[], size_t data_len) {};

    void Draw(ProcessData* data, TFT_eSprite* sprite, bool portrait) {}

  private:
    static bool registered;
};

UI_REGISTER(Empty, 0x00)

}

#endif