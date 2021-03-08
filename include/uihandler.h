#ifndef UIHANDLER_H_
#define UIHANDLER_H_

#include <list>

#include "uielement.h"

namespace euc {

class UiHandler {
  public:
    UiHandler();

    void Update();

  private:
    void LoadFromPreferences();
    void LoadFromData(uint8_t data[], size_t data_len);

    std::list<UiElement> draw_list = std::list<UiElement>();  // Stores the elements in their draw order
};

}

#endif