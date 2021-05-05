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
      min = static_cast<double>(kConstant_args[0]);
      max = static_cast<double>(kConstant_args[1]);
      kConstant_args[2] = std::min(static_cast<uint8_t>(100), kConstant_args[2]); // Limiting these args to 100
      kConstant_args[3] = std::min(static_cast<uint8_t>(100), kConstant_args[3]);

      if (kConstant_args[0] >= kConstant_args[1])
        invalid = true;
    }

    void Draw(ProcessData* data, TFT_eSprite* sprite, bool portrait) {
      if (invalid)  // Do nothing if minimum is bigger than maximum
        return;

      if (first_draw) {  // Calculate these the first time they're used
        if (portrait) {
          size = (kConstant_args[2] / 100.0) * sprite->getViewportWidth();
          position = (kConstant_args[3] / 100.0) * sprite->getViewportWidth();
        } else {
          size = (kConstant_args[2] / 100.0) * sprite->getViewportHeight();
          position = (kConstant_args[3] / 100.0) * sprite->getViewportHeight();
        }
        first_draw = false;
      }

      double scale = (std::min(std::max(data->getDoubleData(kDataType_args[0]), min), max) - min) / range;
      if (portrait)
        sprite->fillRoundRect(position, sprite->getViewportHeight() - (sprite->getViewportHeight() * scale), size, sprite->getViewportHeight() * scale, radius, kColour_args[0](data));
      else
        sprite->fillRoundRect(0, position, sprite->getViewportWidth() * scale, size, radius, kColour_args[0](data));
    }
  
  private:
    int32_t size = 0;
    int32_t position = 0;
    bool first_draw = true, invalid = false;
    double range = 0, min = 0, max = 0;
    static const int32_t radius = 3;
};

UI_REGISTER(BarGraph, 0x04)

}

#endif