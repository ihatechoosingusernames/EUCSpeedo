#ifndef CONFIGSERVER_H_
#define CONFIGSERVER_H_

#include <WebServer.h>

#include "uihandler.h"

namespace euc {

class ConfigServer {
  public:
    ConfigServer(UiHandler* ui_handler);
    ~ConfigServer();

    void Start();
    void Stop();
    void Process();
    
    bool isStarted();

  private:
    void HandleRoot();
    void HandleNotFound();

    bool started = false, connected = false;

    WebServer server;

    UiHandler* ui_handler;
};

}

#endif