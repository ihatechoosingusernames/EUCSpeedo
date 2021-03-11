#include "configserver.h"

#include <functional>

#include "constants.h"

namespace euc {

ConfigServer::ConfigServer(UiHandler* ui_handler) : server(kDefaultServerPort), ui_handler(ui_handler) {
  server.on("/", std::bind(&ConfigServer::HandleRoot, this));
  server.onNotFound(std::bind(&ConfigServer::HandleNotFound, this));
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
  server.stop();
  WiFi.disconnect();
}

void ConfigServer::Process() {
  if (!started) {
    return;
  }
  server.handleClient();
}

bool ConfigServer::isStarted() {
  return started;
}

void ConfigServer::HandleRoot() {
  printf("ConfigServer::HandleRoot()\n");
  server.send(200, "text/plain", "Hello!");
}

void ConfigServer::HandleNotFound() {
  printf("ConfigServer::HandleNotFound()\n");
  server.send(404, "text/plain", "Uh Ohs!");
}

}