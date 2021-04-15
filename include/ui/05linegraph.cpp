#ifndef UI_05LINEGRAPH_CPP_
#define UI_05LINEGRAPH_CPP_

#include "uielement.h"

#include <list>

namespace euc {

class LineGraph : public UiElement {
  UI_SETUP(LineGraph, 0x05, ArgType::kColour, ArgType::kDataType, ArgType::kConstant, ArgType::kConstant)
  UI_ARGS("Colour", "Data", "Minimum", "Maximum")

  public:
    void onCreate() {
      if (kConstant_args[1] > kConstant_args[0])  // Ensure maximum is smaller than minimum
        range = kConstant_args[1] - kConstant_args[0];
    }

    void Draw(ProcessData* data, TFT_eSprite* sprite) {
      if (range == 0)
        return;

      if (!line_data.size())
        line_data.resize(sprite->getViewportWidth(), 255);

      // Limit the data to between the min and max, then subtract the min
      double new_data = std::min(std::max(data->getDoubleData(kDataType_args[0]), kConstant_args[0]), kConstant_args[1]) - kConstant_args[0];

      // Calculate and store the pixel position of this data point
      line_data.emplace_back(sprite->getViewportHeight() - ((new_data / range) * sprite->getViewportHeight()));
      line_data.pop_front();

      uint32_t colour = sprite->color16to24(kColour_args[0](data));

      int32_t x_pos = 0;
      for (uint8_t data_point : line_data) {
        sprite->fillCircle(x_pos, data_point, 2, colour);
        x_pos++;
      }
      
    }

  private:
    std::list<uint8_t> line_data;
    double range = 0;
};

UI_REGISTER(LineGraph, 0x05)

}

#endif