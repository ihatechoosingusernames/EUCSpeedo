#ifndef UI_02BIGNUM_H_
#define UI_02BIGNUM_H_

#include "uielement.h"

namespace euc {

class BigNum : public UiElement {
  UI_SETUP(BigNum, ArgType::kColour, ArgType::kDataType)  // Ensure that the arguments are in the correct order
  
  public:
    BigNum(uint8_t data[], size_t data_len) {
      assert (data[0] == 0x02);
      colour_provider = UiElement::getColourProvider(data, data_len, &data_size);
      type = (DataType)data[data_size++]; // Get type and increment to data_size
    };

    void Draw(ProcessData* data, TFT_eSprite* sprite) {
      printf("BigNum::Draw()\n");
      String num = String(data->getDoubleData(type));  // Retrieve the data

      // Set the background colour to whatever's in the center of the sprite
      sprite->setTextColor(colour_provider(data), sprite->readPixel(sprite->getViewportWidth() / 2, sprite->getViewportHeight() / 2));
      sprite->setTextDatum(MC_DATUM);
      sprite->drawString(num, sprite->getViewportWidth() / 2, sprite->getViewportHeight() / 2, 7);
    };

  private:
    ColourProvider colour_provider;
    DataType type;
};

UI_REGISTER(BigNum, 0x02)

}

#endif