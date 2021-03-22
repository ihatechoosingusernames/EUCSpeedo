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

  server.on("/style.css", HTTP_GET, [this](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/style.css", "text/css");
  });

  server.on("/new_element", HTTP_POST, std::bind(&ConfigServer::ProcessNewElementRequest, this, std::placeholders::_1));

  server.on("/save_changes", HTTP_POST, [this](AsyncWebServerRequest *request){
    // Convert test data to a CSV string and store it in the ui data file
    String ui_data_string;
    for (uint8_t byte : test_ui_data) {
      ui_data_string += String(byte) + ", ";
    }
    String ui_screen_prefs = kUiScreenFilePrefix + String(ui_screen) + "." + kUiScreenFileType;
    file_handler->WriteFile(ui_screen_prefs.c_str(), ui_data_string.c_str());

    request->send(SPIFFS, "/ui_settings.html", "text/html", false, std::bind(&ConfigServer::ProcessUiPage, this, std::placeholders::_1));
  });

  // Create the element selection and data strings for the ui_settings.html template
  for (size_t ui_code = 0; ui_code < kMaxUiElementCode; ui_code++) {
    uint8_t data[] = {static_cast<uint8_t>(ui_code)};
    UiElement* elem = UiElement::Factory(data, 1);

    // This means the code was not valid. We could return here, but the ui namespace may not be contiguous
    if (String(elem->Name()) == String("Empty")) {
      delete elem;
      continue;
    }

    ui_elem_select += "<option value=\"" + String(ui_code) + "\">" + elem->Name() + "</option>\n";

    ui_elem_data += "\"" + String(ui_code) + "\": [";

    for (ArgType arg : elem->ArgList()) {
      ui_elem_data += "\"" + String(static_cast<uint8_t>(arg)) + "\",";
    }

    ui_elem_data += "],";

    delete elem;
  }

  // Create the data type select string for the ui_settings.html template
  for (uint8_t data_type = 0; data_type < static_cast<uint8_t>(DataType::kLastValue); data_type++) {
    ui_data_type_select += "<option value=\"" + String(data_type) + "\">" + kDataTypeNames[data_type] + "</option>\n";
  }

  // If there are saved UI preferences, load them.
  String ui_screen_prefs = kUiScreenFilePrefix + String(ui_screen) + "." + kUiScreenFileType;

  if (file_handler->FileSize(ui_screen_prefs.c_str())) {
    test_ui_data = file_handler->ReadCsvBytes(ui_screen_prefs.c_str());
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
  started = true;
  WiFi.softAP(kDefaultServerSSID);
  Serial.println(WiFi.softAPIP());
  server.begin();
}

void ConfigServer::Stop() {
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

  if (placeholder == "UI_ELEM_DATA") {
    // Replace with UI element code and args in the form "1": ["0", "1"],
    out = ui_elem_data;
  } else if (placeholder == "UI_ELEM_SELECT") {
    // Replace with UI element code and name in the format <option value="1">Background</option>
    out = ui_elem_select;
  } else if (placeholder == "UI_DATA_TYPE_SELECT") {
    // Replace with Data Type code and name in the format <option value="1">Time</option>
    out = ui_data_type_select;
  } else if (placeholder == "UI_DRAW_QUEUE_TABLE") {
    // Replace with table rows representing each UI Element in the current draw list in the format:
    // <tr><td>Background</td></tr>
    for (UiElement* draw_elem : *ui_handler->getDrawList())
      out += "<tr><td>" + String(draw_elem->Name()) + "</td></tr>\n";
  } else if (placeholder == "UI_DATA_TABLE") {
    // Replace with table rows representing each type of data that is being displayed by the UI in
    // the format: <tr><td>Speed</td></tr>
    for (size_t data_type = 0; data_type < (size_t)DataType::kLastValue; data_type++)
      if (test_data_types[data_type])
        out += "<tr><td>" + String(kDataTypeNames[(size_t)data_type]) + "</td></tr>\n";
  }

  return out;
}

void ConfigServer::ProcessNewElementRequest(AsyncWebServerRequest *request) {
  printf("New Element created\n");

  int params = request->params();
  for(int i=0;i<params;i++){
    AsyncWebParameter* p = request->getParam(i);
    Serial.printf("%s: %s\n", p->name().c_str(), p->value().c_str());
  }

  bool has_error = false;
  uint8_t elem_type;

  if (request->hasParam("ui_element", true))
    elem_type = std::atoi(request->getParam("ui_element", true)->value().c_str());
  else
    has_error = true;

  UiElement* elem = UiElement::Factory(&elem_type, 1);
  std::list<ArgType> args = elem->ArgList();
  std::list<uint8_t> data = {elem_type};

  delete elem;  // Clean up to avoid memory leaks

  for (ArgType arg : args) {
    switch (arg) {
      case ArgType::kDataType:
        if (request->hasParam("data_arg", true)) {
          data.emplace_back(std::atoi(request->getParam("data_arg", true)->value().c_str()));
          test_data_types[std::atoi(request->getParam("data_arg", true)->value().c_str())] = true;  // Mark this datatype as being used
        } else {
          has_error = true;
        }
        break;
      case ArgType::kColour:
        if (request->hasParam("colour_arg", true))
          if (request->getParam("colour_arg", true)->value() == String((int)ColourType::kConstant)) {  // Solid Colour
            data.emplace_back((uint8_t)ColourType::kConstant);
            if (request->hasParam("solid_colour_arg", true)) {
              std::list<uint8_t> parsed_colour = ParseColour(request->getParam("solid_colour_arg", true)->value());
              if (parsed_colour.size() < 3)
                has_error = true;
              else
                data.insert(data.end(), parsed_colour.begin(), parsed_colour.end());
            } else {
              has_error = true;
            }
          } else if (request->getParam("colour_arg", true)->value() == String((int)ColourType::kDynamicBetweenValues)) { // Dynamic Colour
            data.emplace_back((uint8_t)ColourType::kDynamicBetweenValues);
            if (request->hasParam("colour_data_type", true) && request->hasParam("arg_low_data", true) && request->hasParam("arg_low_colour", true)
                && request->hasParam("arg_high_data", true) && request->hasParam("arg_high_colour", true)) {
              data.emplace_back(std::atoi(request->getParam("colour_data_type", true)->value().c_str()));
              test_data_types[std::atoi(request->getParam("colour_data_type", true)->value().c_str())] = true;  // Mark this datatype as being used

              data.emplace_back(std::atoi(request->getParam("arg_low_data", true)->value().c_str()));
              data.emplace_back(std::atoi(request->getParam("arg_high_data", true)->value().c_str()));

              std::list<uint8_t> parsed_low_colour = ParseColour(request->getParam("arg_low_colour", true)->value());
              std::list<uint8_t> parsed_high_colour = ParseColour(request->getParam("arg_high_colour", true)->value());

              if (parsed_low_colour.size() < 3 || parsed_high_colour.size() < 3)
                has_error = true;

              data.insert(data.end(), parsed_low_colour.begin(), parsed_low_colour.end());
              data.insert(data.end(), parsed_high_colour.begin(), parsed_high_colour.end());
            }
          }
        else
          has_error = true;
        break;
      case ArgType::kConstant:
        if (request->hasParam("constant_arg", true))
          data.emplace_back(std::atoi(request->getParam("constant_arg", true)->value().c_str()));
        else
          has_error = true;
        break;
    }
  }

  if (!has_error) {
    printf("Inserting new element\n");

    printf("Data: ");
    for (uint8_t byte : data)
      printf("%d, ", (int)byte);

    printf("\n");

    // Appending this new element to the test data.
    test_ui_data.insert(test_ui_data.end(), data.begin(), data.end());
    uint8_t new_data[test_ui_data.size()];
    size_t counter = 0;
    for (uint8_t byte : test_ui_data)
      new_data[counter++] = byte;

    ui_handler->LoadFromData(new_data, counter);
  } else {
    printf("Element has error\n");
    printf("Data: ");
    for (uint8_t byte : data)
      printf("%d, ", (int)byte);

    printf("\n");
    // Handle notifying user of errors here
  }

  // Resend the updated UI page
  request->send(SPIFFS, "/ui_settings.html", "text/html", false, std::bind(&ConfigServer::ProcessUiPage, this, std::placeholders::_1));
}

std::list<uint8_t> ConfigServer::ParseColour(String colour) {
  std::list<uint8_t> out;
  // Colour is represented by a string like "#ff00bb" where each pair of digits is a number representing
  // red (0xff), green (0x00), and blue (0xbb) in hexadecimal

  if (colour.length() < 7) {
    return out;
  }
  
  // Converts from string to unsigned long in base 16
  out.emplace_back(std::strtoul(colour.substring(1, 3).c_str(), NULL, 16)); // Red
  out.emplace_back(std::strtoul(colour.substring(3, 5).c_str(), NULL, 16)); // Green
  out.emplace_back(std::strtoul(colour.substring(5).c_str(), NULL, 16)); // Blue

  return out;
}

}