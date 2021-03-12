#include "configserver.h"

#include <functional>

#include "constants.h"

namespace euc {

ConfigServer::ConfigServer(UiHandler* ui_handler) : server(kDefaultServerPort), ui_handler(ui_handler) {
  server.on("/", HTTP_GET, std::bind(&ConfigServer::HandleRoot, this, std::placeholders::_1));
}

ConfigServer::~ConfigServer() {
  if (started)
    Stop();
}

void ConfigServer::Start() {
  printf("ConfigServer::Start()\n");
  started = true;
  WiFi.softAP(kDefaultServerSSID);
  Serial.println(WiFi.softAPIP());
  server.begin();
}

void ConfigServer::Stop() {
  printf("ConfigServer::Stop()\n");
  started = false;
  server.end();
  WiFi.disconnect();
}

bool ConfigServer::isStarted() {
  return started;
}

void ConfigServer::HandleRoot(AsyncWebServerRequest *request) {
  printf("ConfigServer::HandleRoot()\n");
  request->send(200, "text/plain", "Hello!");
}

}