#include "configserver.h"

#include "SPIFFS.h"

#include "constants.h"

namespace euc {

ConfigServer::ConfigServer(UiHandler* ui_handler) : server(kDefaultServerPort), ui_handler(ui_handler) {
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/ui_settings.html", "text/html");
  });


  server.on("/favicon.ico", HTTP_GET, [](AsyncWebServerRequest *request){
    printf("Favicon requested\n");
    request->send(SPIFFS, "/favicon.png","image/png");
  });
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

}