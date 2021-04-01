#ifndef UI_04BARGRAPH_H_
#define UI_04BARGRAPH_H_

#include "uielement.h"

namespace euc {

class BarGraph : public UiElement {
  UI_SETUP(BarGraph, 0x04, ArgType::kColour, ArgType::kDataType, ArgType::kConstant, ArgType::kConstant)
  UI_ARGS("Graph Colour", "Graph Data", "Graph Data Minimum", "Graph Data Maximum")

  public:
    void Draw(ProcessData* data, TFT_eSprite* sprite) {
      if (kConstant_args[0] > kConstant_args[1])  // Do nothing if minimum is bigger than maximum
        return;

      double range = kConstant_args[1] - kConstant_args[0];
      double scale = (std::min(std::max(data->getDoubleData(kDataType_args[0]), kConstant_args[0]), kConstant_args[1]) - kConstant_args[0]) / range;
      sprite->fillRoundRect(0, 0, sprite->getViewportWidth() * scale, sprite->getViewportHeight(), 3, kColour_args[0](data));
    }
};

UI_REGISTER(BarGraph, 0x04)

}

#endif