#ifndef UI_02BIGNUM_H_
#define UI_02BIGNUM_H_

#include "uielement.h"

namespace euc {

class BigNum : public UiElement {
  UI_SETUP(BigNum, 0x02, ArgType::kColour, ArgType::kDataType, ArgType::kConstant); // Ensure that the arguments are in the correct order
  UI_ARGS("Colour", "Data Type", "Position (0 - 2)");
  
  public:
    void onCreate() {
      kConstant_args[0] %= 3; // Max size for this arg is 2
    }

    void Draw(ProcessData* data, TFT_eSprite* sprite) {
      if (first_draw) {
        if (!kConstant_args[0]) {
          position[0] = 0;
          datum = ML_DATUM;
        } else if (kConstant_args[0] == 1) {
          position[0] = sprite->getViewportWidth() / 2;
          datum = MC_DATUM;
        } else {
          position[0] = sprite->getViewportWidth();
          datum = MR_DATUM;
        }
        
        position[1] = sprite->getViewportHeight() / 2;
        first_draw = false;
      }
      
      if (sprite->fontsLoaded())
        sprite->unloadFont();

      // Set the main colour, and the background colour to whatever's behind the number
      sprite->setTextColor(kColour_args[0](data), sprite->readPixel(position[0], position[1]));
      sprite->setTextDatum(datum);
      sprite->drawNumber(data->getDoubleData(kDataType_args[0]), position[0], position[1], kBigNumberFont);
    };

  private:
    uint8_t position[2] = {0, 0};
    uint8_t datum = 0;
    bool first_draw = true;
};

UI_REGISTER(BigNum, 0x02)

}

#endif