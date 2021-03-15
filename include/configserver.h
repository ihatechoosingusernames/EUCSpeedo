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
    ConfigServer(UiHandler* ui_handler, FileHandler* files);
    ~ConfigServer();

    void Start();
    void Stop();
    
    bool isStarted();

  private:
    String ProcessUiPage(const String& placeholder);
    void ProcessNewElementRequest(AsyncWebServerRequest *request);

    std::list<uint8_t> ParseColour(String colour);
    
    bool started = false, connected = false;

    AsyncWebServer server;

    UiHandler* ui_handler;
    FileHandler* file_handler;

    std::list<uint8_t> test_ui_data;
    bool test_data_types[(size_t)DataType::kLastValue] = {{false}};

    String ui_elem_data, ui_elem_select, ui_data_type_select;
};

}

#endif