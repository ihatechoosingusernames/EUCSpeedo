#ifndef CONFIGSERVER_H_
#define CONFIGSERVER_H_

#include "ESPAsyncWebServer.h"
#include "AsyncTCP.h"

#include "uihandler.h"
#include "filehandler.h"

namespace euc {

/*

  This async server is handled internally as synchronous, as only one connection at a time is allowed.
  It of course is not synchronous with respect to outside data.

*/

class ConfigServer {
  public:
    ConfigServer(UiHandler* arg_ui_handler, FileHandler* files);
    ~ConfigServer();

    void Start();
    void Stop();
    
    bool isStarted();

  private:
    String ProcessUiPage(const String& placeholder);
    void ProcessNewElementRequest(AsyncWebServerRequest *request);

    std::vector<uint8_t> ParseColour(String colour);
    void RemoveElement(size_t index);
    void ReorderElement(size_t index, int move);
    void ReloadTestData();
    
    bool started = false;

    AsyncWebServer server;

    UiHandler* ui_handler;
    FileHandler* file_handler;

    std::vector<std::vector<uint8_t>> test_ui_data; // Stores the data that represents each UI element in draw order
    ProcessData test_process_data;

    bool test_data_types[(size_t)DataType::kLastValue] = {{false}};

    String ui_elem_data, ui_elem_select, ui_data_type_select;

    uint8_t ui_screen;
};

}

#endif