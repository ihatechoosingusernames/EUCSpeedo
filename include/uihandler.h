#ifndef UIHANDLER_H_
#define UIHANDLER_H_

#include <list>

#include "uielement.h"
#include "processdata.h"
#include "filehandler.h"

namespace euc {

class UiHandler {
  public:
    UiHandler(FileHandler* file_handler);
    ~UiHandler();

    void Update(ProcessData* data);
    void LoadFromData(uint8_t data[], size_t data_len);

  private:
    void LoadFromPreferences(FileHandler* file_handler);

    std::list<UiElement*> draw_list = std::list<UiElement*>();  // Stores the elements in their draw order
};

}

#endif