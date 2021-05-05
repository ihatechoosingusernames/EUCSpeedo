#ifndef UI_03_LITTLENUM_CPP_
#define UI_03_LITTLENUM_CPP_

#include "uielement.h"

namespace euc {

class LittleNum : public UiElement {
  UI_SETUP(LittleNum, 0x03, ArgType::kColour, ArgType::kDataType, ArgType::kConstant)
  UI_ARGS("Number Colour", "Data Type", "Position (0 - 5)")

  public:
    void Draw(ProcessData* data, TFT_eSprite* sprite, bool portrait) {
      sprite->loadFont(kSmallTextFont);

      if (first_draw) {
        if (portrait) {
          position[1] = ((sprite->getViewportHeight() / 3) * (static_cast<int32_t>(kConstant_args[0]) % 3)) + ((sprite->getViewportHeight() / 3) / 2);
          position[0] = (static_cast<int32_t>(kConstant_args[0]) > 2 ? padding : static_cast<int32_t>(sprite->getViewportWidth()) - padding);
          datum = (static_cast<int32_t>(kConstant_args[0]) > 2 ? ML_DATUM : MR_DATUM);
        } else {
          position[0] = ((sprite->getViewportWidth() / 3) * (static_cast<int32_t>(kConstant_args[0]) % 3)) + ((sprite->getViewportWidth() / 3) / 2);
          position[1] = (static_cast<int32_t>(kConstant_args[0]) > 2 ? padding : static_cast<int32_t>(sprite->getViewportHeight() - sprite->fontHeight(sprite->fontsLoaded())) - padding);
          datum = (static_cast<int32_t>(kConstant_args[0]) > 2 ? BC_DATUM : TC_DATUM);
        }
        first_draw = false;
      }
      
      sprite->setTextColor(kColour_args[0](data), sprite->readPixel(position[0], position[1]));
      sprite->setTextDatum(datum);
      sprite->drawNumber(data->getDoubleData(kDataType_args[0]), position[0], position[1]);
      sprite->unloadFont();
    };

  private:
    int32_t position[2] = {0, 0};
    uint8_t datum = 0;
    bool first_draw = true;
    static const int32_t padding = 3, portrait_padding = 10;
};

UI_REGISTER(LittleNum, 0x03);

}

#endif