#ifndef UI_05LINEGRAPH_CPP_
#define UI_05LINEGRAPH_CPP_

#include "uielement.h"

namespace euc {

class LineGraph : public UiElement {
  UI_SETUP(LineGraph, 0x05, ArgType::kColour, ArgType::kDataType, ArgType::kConstant, ArgType::kConstant)
  UI_ARGS("Colour", "Data", "Minimum", "Maximum")

  public:
    void Draw(ProcessData* data, TFT_eSprite* sprite) {

    }

  private:
    uint8_t line_data[];
};

UI_REGISTER(LineGraph, 0x05)

}

#endif