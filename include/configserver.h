#ifndef CONFIGSERVER_H_
#define CONFIGSERVER_H_

#include "ESPAsyncWebServer.h"
#include "AsyncTCP.h"

#include "uihandler.h"

namespace euc {

class ConfigServer {
  public:
    ConfigServer(UiHandler* ui_handler);
    ~ConfigServer();

    void Start();
    void Stop();
    
    bool isStarted();

  private:
    bool started = false, connected = false;

    AsyncWebServer server;

    UiHandler* ui_handler;
};

}

#endif