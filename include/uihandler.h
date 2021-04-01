#ifndef UIHANDLER_H_
#define UIHANDLER_H_

#include <list>

#include <TFT_eSPI.h>

#include "uielement.h"
#include "processdata.h"
#include "filehandler.h"
namespace euc {

class UiHandler {
  public:
    UiHandler(FileHandler* file_handler, UiScreen start_screen = UiScreen::kHome);
    ~UiHandler();

    void ChangeScreen(UiScreen new_ui_screen);
    void Update(ProcessData* data);
    void LoadFromData(uint8_t data[], size_t data_len);
    void Sleep();

    UiScreen getCurrentScreen();
    std::vector<UiElement*>* getDrawList();

  private:
    void LoadFromFile(FileHandler* file_handler);
    void ClearDrawList();

    std::vector<UiElement*> draw_list;  // Stores the elements in their draw order
    TFT_eSPI screen;  // Actual Screen object

    UiScreen ui_screen; // Which 'screen' of the UI is currently being displayed
    FileHandler* file_handler;

    const char* font_name = "FiraCodeBold20";  // TODO: Maybe put in constants?
    // const char* font_filename = "/FiraCodeBold20.vlw";
};

}

#endif