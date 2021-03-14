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
    
    bool started = false, connected = false;

    AsyncWebServer server;

    UiHandler* ui_handler;
    FileHandler* file_handler;

    std::list<uint8_t> test_ui_data;
};

}

#endif