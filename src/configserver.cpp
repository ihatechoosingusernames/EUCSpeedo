#include "configserver.h"

#include <functional>
#include "SPIFFS.h"

#include "constants.h"
#include "uielement.h"

namespace euc {

ConfigServer::ConfigServer(UiHandler* ui_handler, FileHandler* files) : server(kDefaultServerPort), ui_handler(ui_handler), file_handler(files) {
  server.on("/", HTTP_GET, [this](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/ui_settings.html", "text/html", false, std::bind(&ConfigServer::ProcessUiPage, this, std::placeholders::_1));
  });

  server.on("/new_element", HTTP_POST, [this](AsyncWebServerRequest *request){
    printf("New Element created\n");

    bool has_error = false;
    uint8_t elem_type = 0;

    if (request->hasParam("ui_element"))
      elem_type = std::atoi(request->getParam("ui_element")->value().c_str());
    else
      has_error = true;

    std::list<ArgType> args = UiElement::Factory(&elem_type, 1)->ArgList();
    std::list<uint8_t> data;

    for (ArgType arg : args) {
      switch (arg) {
        case ArgType::kDataType:
          if (request->hasParam("data_arg"))
            data.emplace_back(std::atoi(request->getParam("data_arg")->value().c_str()));
          else
            has_error = true;
          break;
        case ArgType::kColour:
          if (request->hasParam("colour_arg"))
            data.emplace_back(std::atoi(request->getParam("colour_arg")->value().c_str()));
          else
            has_error = true;
          break;
        case ArgType::kConstant:
          if (request->hasParam("constant_arg"))
            data.emplace_back(std::atoi(request->getParam("constant_arg")->value().c_str()));
          else
            has_error = true;
          break;
      }
    }

    data.emplace_front(elem_type);

    if (!has_error) {
      // Appending this new element to the test data.
      test_ui_data.insert(test_ui_data.end(), data.begin(), data.end());
    } else {
      // Handle notifying user of errors here
    }

    // Resend the updated UI page
    request->send(SPIFFS, "/ui_settings.html", "text/html", false, std::bind(&ConfigServer::ProcessUiPage, this, std::placeholders::_1));
  });

  server.on("/save_changes", HTTP_POST, [this](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/ui_settings.html", "text/html", false, std::bind(&ConfigServer::ProcessUiPage, this, std::placeholders::_1));
  });

  // If there are saved UI preferences, load them.
  if (file_handler->FileSize(kUiPreferences)) {
    test_ui_data = file_handler->ReadCsvBytes(kUiPreferences);
  } else {  // If not, use the "factory" settings
    for (size_t counter = 0; counter < kUiDefaultPreferencesLength; counter++)
      test_ui_data.push_back(kUiDefaultPreferences[counter]);
  }
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

// Processing the ui_settings.html template page and replacing placeholders with real data
String ConfigServer::ProcessUiPage(const String& placeholder) {
  String out = "";

  Serial.println("Processing placeholder " + placeholder + "\n");

  if (placeholder == "UI_ELEM_DATA") {
    // Replace with UI element code and args in the form "1": ["0", "1"],
    for (size_t ui_code = 0; ui_code < kMaxUiElementCode; ui_code++) {
      uint8_t data[] = {static_cast<uint8_t>(ui_code)};
      UiElement* elem = UiElement::Factory(data, 1);

      // This means the code was not valid. We could return here, but the ui namespace may not be contiguous
      if (elem->Name() == "Empty") {
        continue;
      }

      out += "\"" + String(ui_code) + "\": [";

      for (ArgType arg : elem->ArgList()) {
        out += String(static_cast<uint8_t>(arg)) + ",";
      }

      out += "],";
    }
  } else if (placeholder == "UI_ELEM_SELECT") {
    // Replace with UI element code and name in the format <option value="1">Background</option>
    for (size_t ui_code = 0; ui_code < kMaxUiElementCode; ui_code++) {
      uint8_t data[] = {static_cast<uint8_t>(ui_code)};
      UiElement* elem = UiElement::Factory(data, 1);

      // This means the code was not valid. We could return here, but the ui namespace may not be contiguous
      if (elem->Name() == "Empty") {
        continue;
      }

      out += "<option value=\"" + String(ui_code) + "\">" + elem->Name() + "</option>\n";
    }
  } else if (placeholder == "UI_DATA_TYPE_SELECT") {
    // Replace with Data Type code and name in the format <option value="1">Time</option>
    for (uint8_t data_type = 0; data_type < static_cast<uint8_t>(DataType::kLastValue); data_type++) {
      out += "<option value=\"" + String(data_type) + "\">" + kDataTypeNames[data_type] + "</option>\n";
    }
  }

  Serial.println("Returning:\n" + out);

  return out;
}

}