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
      
      min = static_cast<double>(kConstant_args[0]);
      max = static_cast<double>(kConstant_args[1]);
    }

    void Draw(ProcessData* data, TFT_eSprite* sprite, bool portrait) {
      if (range == 0)
        return;

      if (!line_data.size())
        line_data.resize(sprite->getViewportWidth(), 255);

      // Limit the data to between the min and max, then subtract the min
      double new_data = std::min(std::max(data->getDoubleData(kDataType_args[0]), min), max) - min;

      // Calculate and store the pixel position of this data point
      line_data[(++line_data_pos %= line_data.size())] = (sprite->getViewportHeight() - ((new_data / range) * sprite->getViewportHeight()));

      uint32_t colour = sprite->color16to24(kColour_args[0](data));

      int32_t x_pos = 0;
      // Iterating backwards through the line_data vector as the information is updated forwards
      for (size_t data_point = (line_data_pos + 1) % line_data.size(); data_point != line_data_pos; ++data_point %= line_data.size()) {
        sprite->fillCircle(x_pos, line_data.at(data_point), 2, colour);
        x_pos++;
      }
      
    }

  private:
    std::vector<uint8_t> line_data;
    double range = 0, min = 0, max = 0;
    size_t line_data_pos = 0;
};

UI_REGISTER(LineGraph, 0x05)

}

#endif