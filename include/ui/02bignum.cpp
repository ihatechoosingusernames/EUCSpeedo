#ifndef UI_02BIGNUM_H_
#define UI_02BIGNUM_H_

#include "uielement.h"

namespace euc {

class BigNum : public UiElement {
  UI_SETUP(BigNum, 0x02, ArgType::kColour, ArgType::kDataType)  // Ensure that the arguments are in the correct order
  
  public:
    void Draw(ProcessData* data, TFT_eSprite* sprite) {
      printf("BigNum::Draw()\n");
      String num = String(data->getDoubleData(kDataType_args[0]), 1);  // Retrieve the data, 1 decimal place
      num = num.substring(0, std::min(num.length(), static_cast<unsigned int>(2))); // Limit it to two digits

      // Set the background colour to whatever's in the center of the sprite
      sprite->setTextColor(kColour_args[0](data), sprite->readPixel(sprite->getViewportWidth() / 2, sprite->getViewportHeight() / 2));
      sprite->setTextDatum(MC_DATUM);
      sprite->drawString(num, sprite->getViewportWidth() / 2, sprite->getViewportHeight() / 2, 7);
    };
};

UI_REGISTER(BigNum, 0x02)

}

#endif