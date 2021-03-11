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

    void Draw(ProcessData* data) {
      printf("BigNum::Draw()\n");
      double num = data->getDoubleData(type);  // Retrieve the data
    };

  private:
    ColourProvider colour_provider;
    DataType type;
};

UI_REGISTER(BigNum, 0x02)

}

#endif