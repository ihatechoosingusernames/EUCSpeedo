#ifndef UI_01BACKGROUND_H_
#define UI_01BACKGROUND_H_

#include "uielement.h"

namespace euc {

class Background : public UiElement {
  UI_SETUP(Background, ArgType::kColour)  // Declaring arguments in correct order to help with config generation
  
  public:
    Background(uint8_t data[], size_t data_len) {
      assert(data[0] == 0x01);  // First check that the first piece of data is the code of this UI element 
      colour_provider = UiElement::getColourProvider(&data[1], data_len - 1, &data_size);
      data_size += 1;  // Add 1 for the name
    };

    void Draw(ProcessData* data) {
      printf("Background::Draw() with colour %X\n", colour_provider(data));
    };

  private:
    ColourProvider colour_provider;
};

UI_REGISTER(Background, 0x01)

}

#endif