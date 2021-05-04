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
    UiHandler(FileHandler* file_handler, uint8_t start_screen = 0);
    ~UiHandler();

    void ChangeScreen(uint8_t new_ui_screen);
    void Update(ProcessData* data);
    void LoadFromData(uint8_t data[], size_t data_len);
    void Sleep();
    void Wake();
    void ShowMessage(const char* message, double time); // Message to display, time to display it for in seconds
    void HandleInput(PressType type);

    uint8_t getCurrentScreen();
    std::vector<UiElement*>* getDrawList();

  private:
    void LoadFromFile();
    void ClearDrawList();

    std::vector<UiElement*> draw_list;  // Stores the elements in their draw order
    TFT_eSPI screen;  // Actual Screen object

    uint8_t ui_screen; // Which 'screen' of the UI is currently being displayed
    FileHandler* file_handler;

    String message;
    unsigned long message_timeout = 0;
};

}

#endif