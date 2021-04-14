#ifndef UI_03_LITTLENUM_CPP_
#define UI_03_LITTLENUM_CPP_

#include "uielement.h"

namespace euc {

class LittleNum : public UiElement {
  UI_SETUP(LittleNum, 0x03, ArgType::kColour, ArgType::kDataType, ArgType::kConstant)
  UI_ARGS("Number Colour", "Data Type", "Position (0 - 5)")

  public:
    void Draw(ProcessData* data, TFT_eSprite* sprite) {
      sprite->loadFont(kSmallTextFont);

      if (!(position[0] || position[1])) {  // Should only happen on first draw
        position[0] = ((sprite->getViewportWidth() / 3) * (static_cast<int32_t>(kConstant_args[0]) % 3)) + ((sprite->getViewportWidth() / 3) / 2);
        position[1] = (static_cast<int32_t>(kConstant_args[0]) > 2 ? padding : static_cast<int32_t>(sprite->getViewportHeight() - sprite->fontHeight(sprite->fontsLoaded())) - padding);
      }
      
      sprite->setTextColor(kColour_args[0](data), sprite->readPixel(position[0], position[1]));
      sprite->setTextDatum(TC_DATUM);
      sprite->drawNumber(data->getDoubleData(kDataType_args[0]), position[0], position[1]);
      sprite->unloadFont();
    };

  private:
    int32_t position[2] = {0, 0};
    static const int32_t padding = 3;
};

UI_REGISTER(LittleNum, 0x03);

}

#endif