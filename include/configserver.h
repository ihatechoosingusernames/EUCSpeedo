#ifndef CONFIGSERVER_H_
#define CONFIGSERVER_H_

#include "ESPAsyncWebServer.h"
#include "AsyncTCP.h"

#include "uihandler.h"
#include "filehandler.h"
#include "settings.h"

namespace euc {

class ConfigServer {
  public:
    ConfigServer(UiHandler* arg_ui_handler, FileHandler* files, RtcHandler* rtc, Settings* settings);
    ~ConfigServer();

    void Start();
    void Stop();
    
    bool isStarted();

  private:
    String ProcessUiPage(const String& placeholder);
    String ProcessSettingsPage(const String& placeholder);

    void ProcessNewElementRequest(AsyncWebServerRequest *request);
    void ProcessUpdateRequest(AsyncWebServerRequest *request);
    void ProcessUpdateUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final);
    void ProcessUpdateSettings(AsyncWebServerRequest *request);

    std::vector<uint8_t> ParseColour(String colour);

    void LoadTestData(uint8_t screen);
    void RemoveElement(size_t index);
    void ReorderElement(size_t index, int move);
    void DisplayTestData();
    
    bool started = false, final_upload = false;

    AsyncWebServer server;

    UiHandler* ui_handler;
    FileHandler* file_handler;
    RtcHandler* rtc_handler;
    Settings* settings_handler;

    std::vector<std::vector<uint8_t>> test_ui_data; // Stores the data that represents each UI element in draw order
    ProcessData test_process_data;

    bool test_data_types[static_cast<size_t>(DataType::kLastValue)];

    String ui_elem_data, ui_elem_select, ui_data_type_select;

    uint8_t ui_screen;
};

}

#endif