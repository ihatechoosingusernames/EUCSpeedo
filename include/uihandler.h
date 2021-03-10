#ifndef UIHANDLER_H_
#define UIHANDLER_H_

#include <list>

#include "uielement.h"
#include "processdata.h"

namespace euc {

class UiHandler {
  public:
    UiHandler();

    void Update(ProcessData* data);

  private:
    void LoadFromPreferences();
    void LoadFromData(uint8_t data[], size_t data_len);

    std::list<UiElement*> draw_list = std::list<UiElement*>();  // Stores the elements in their draw order
};

}

#endif