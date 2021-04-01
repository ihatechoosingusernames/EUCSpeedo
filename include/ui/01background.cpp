#ifndef UI_01BACKGROUND_H_
#define UI_01BACKGROUND_H_

#include "uielement.h"

namespace euc {

class Background : public UiElement {
  UI_SETUP(Background, 1, ArgType::kColour)  // Declaring arguments in correct order to help with config generation
  UI_ARGS("Background Colour")  // Optional, but setting the argument names for display in the config server
  
  public:
    void Draw(ProcessData* data, TFT_eSprite* sprite) override {
      sprite->fillSprite(kColour_args[0](data));
    };

  private:
    ColourProvider colour_provider;
};

UI_REGISTER(Background, 0x01)

}

#endif