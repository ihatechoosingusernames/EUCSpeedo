#ifndef CONFIGSERVER_H_
#define CONFIGSERVER_H_

#include "ESPAsyncWebServer.h"
#include "AsyncTCP.h"

#include "uihandler.h"
#include "filehandler.h"

namespace euc {

class ConfigServer {
  public:
    ConfigServer(UiHandler* arg_ui_handler, FileHandler* files, RtcHandler* rtc);
    ~ConfigServer();

    void Start();
    void Stop();
    
    bool isStarted();

  private:
    String ProcessUiPage(const String& placeholder);
    void ProcessNewElementRequest(AsyncWebServerRequest *request);
    void ProcessUpdateRequest(AsyncWebServerRequest *request);
    void ProcessUpdateUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final);

    std::vector<uint8_t> ParseColour(String colour);
    void RemoveElement(size_t index);
    void ReorderElement(size_t index, int move);
    void ReloadTestData();
    
    bool started = false;

    AsyncWebServer server;

    UiHandler* ui_handler;
    FileHandler* file_handler;
    RtcHandler* rtc_handler;

    std::vector<std::vector<uint8_t>> test_ui_data; // Stores the data that represents each UI element in draw order
    ProcessData test_process_data;

    bool test_data_types[(size_t)DataType::kLastValue] = {{false}};

    String ui_elem_data, ui_elem_select, ui_data_type_select;

    uint8_t ui_screen = static_cast<uint8_t>(UiScreen::kCustom);
};

}

#endif