#ifndef UI_03SMALLTEXT_H_
#define UI_03SMALLTEXT_H_

#include "uielement.h"

namespace euc {

class SmallText : public UiElement {
  UI_SETUP(SmallText, 0x03, ArgType::kColour, ArgType::kText)
  UI_ARGS("Text Colour", "Text to Display")

  public:
    void Draw(ProcessData* data, TFT_eSprite* sprite) {
      sprite->loadFont(kSmallTextFont);
      sprite->setTextColor(kColour_args[0](data));
      sprite->printToSprite(kText_args[0]);
    }
};

UI_REGISTER(SmallText, 0x03)

}

#endif