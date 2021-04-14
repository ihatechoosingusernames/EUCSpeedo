#ifndef UI_04BARGRAPH_H_
#define UI_04BARGRAPH_H_

#include "uielement.h"

namespace euc {

class BarGraph : public UiElement {
  UI_SETUP(BarGraph, 0x04, ArgType::kColour, ArgType::kDataType, ArgType::kConstant, ArgType::kConstant, ArgType::kConstant, ArgType::kConstant)
  UI_ARGS("Graph Colour", "Graph Data", "Graph Data Minimum", "Graph Data Maximum", "Graph Width (0 - 100)", "Graph Position (0 - 100)")

  public:
    void onCreate() {
      range = kConstant_args[1] - kConstant_args[0];
    }

    void Draw(ProcessData* data, TFT_eSprite* sprite) {
      if (kConstant_args[0] >= kConstant_args[1])  // Do nothing if minimum is bigger than maximum
        return;

      if (!size && !position) {  // Calculate these the first time they're used
        size = (kConstant_args[2] / 100) * sprite->getViewportHeight();
        position = (kConstant_args[3] / 100) * sprite->getViewportHeight();
      }

      double scale = (std::min(std::max(data->getDoubleData(kDataType_args[0]), kConstant_args[0]), kConstant_args[1]) - kConstant_args[0]) / range;
      sprite->fillRoundRect(0, position, sprite->getViewportWidth() * scale, size, 3, kColour_args[0](data));
    }
  
  private:
    int32_t size = 0;
    int32_t position = 0;
    double range;
};

UI_REGISTER(BarGraph, 0x04)

}

#endif