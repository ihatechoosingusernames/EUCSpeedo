#ifndef UI_02BIGNUM_H_
#define UI_02BIGNUM_H_

#include "uielement.h"

namespace euc {

class BigNum : public UiElement {
  UI_SETUP(BigNum, 0x02, ArgType::kColour, ArgType::kDataType)  // Ensure that the arguments are in the correct order
  
  public:
    void Draw(ProcessData* data, TFT_eSprite* sprite) {
      // Set the background colour to whatever's in the center of the sprite
      sprite->setTextColor(kColour_args[0](data), sprite->readPixel(sprite->getViewportWidth() / 2, sprite->getViewportHeight() / 2));
      sprite->setTextDatum(MC_DATUM);
      sprite->drawNumber(data->getDoubleData(kDataType_args[0]), sprite->getViewportWidth() / 2, sprite->getViewportHeight() / 2, kBigNumberFont);
    };
};

UI_REGISTER(BigNum, 0x02)

}

#endif