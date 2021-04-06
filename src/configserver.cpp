#include "configserver.h"

#include <functional>
#include "SPIFFS.h"
#include <ESPmDNS.h>
#include <Update.h>

#include "constants.h"
#include "uielement.h"
#include "utils.h"
#include "logging.h"

namespace euc {

ConfigServer::ConfigServer(UiHandler* arg_ui_handler, FileHandler* files, RtcHandler* rtc) : server(kDefaultServerPort),
    ui_handler(arg_ui_handler), file_handler(files), rtc_handler(rtc) {
  server.on("/", HTTP_GET, [this](AsyncWebServerRequest *request){
    LOG_DEBUG("/");
    request->send(SPIFFS, "/ui_settings.html", "text/html", false, std::bind(&ConfigServer::ProcessUiPage, this, std::placeholders::_1));
  });

  server.on("/general_settings", HTTP_GET, [this](AsyncWebServerRequest *request){
    LOG_DEBUG("/general_settings");
    request->send(SPIFFS, "/general_settings.html", "text/html", false);
  });

  server.on("/style.css", HTTP_GET, [this](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/style.css", "text/css");
  });

  server.on("/new_element", HTTP_POST, std::bind(&ConfigServer::ProcessNewElementRequest, this, std::placeholders::_1));

  server.on("/save_changes", HTTP_POST, [this](AsyncWebServerRequest *request){
    LOG_DEBUG("/save_changes");
    // Convert test data to a CSV string and store it in the ui data file
    // save_data = true;

    String ui_data_string;
    for (std::vector<uint8_t> byte_vec : test_ui_data) {
      for (uint8_t byte : byte_vec)
        ui_data_string += String(byte) + ", ";
      ui_data_string += "\n";
    }

    LOG_DEBUG_ARGS("Saving: %s\n\tto file %s", ui_data_string.c_str(), Utils::getUiScreenFileName(ui_screen).c_str());

    file_handler->WriteFile(Utils::getUiScreenFileName(ui_screen), ui_data_string);

    request->send(SPIFFS, "/ui_settings.html", "text/html", false, std::bind(&ConfigServer::ProcessUiPage, this, std::placeholders::_1));
  });

  server.on("/remove_element", HTTP_DELETE, [this](AsyncWebServerRequest *request){
    LOG_DEBUG("/remove_element");
    if(request->hasParam("id")) {
      RemoveElement(std::atoi(request->getParam("id")->value().c_str()));
      ReloadTestData();
    }
  });

  server.on("/update_data", HTTP_POST, [this](AsyncWebServerRequest * request){
    LOG_DEBUG("/update_data");
    if (!request->hasParam("id", true))
      return;
    if (!request->hasParam("data", true))
      return;

    LOG_DEBUG(("Updating data type " + request->getParam("id", true)->value() + " with data " + request->getParam("data", true)->value() + "\n").c_str());

    // Get data type as a string, then convert to String -> const char* -> int -> DataType. Very efficient.
    DataType data = static_cast<DataType>(std::atoi(request->getParam("id", true)->value().c_str()));
    double val = std::atof(request->getParam("data", true)->value().c_str());

    test_process_data.Update(data, val);
    ui_handler->Update(&test_process_data);
  });

  server.on("/update_elem_order", HTTP_POST, [this](AsyncWebServerRequest * request){
    LOG_DEBUG("/update_elem_order");
    for (size_t param = 0; param < request->params(); param++) {
      LOG_DEBUG((request->getParam(param)->name() + " : " + request->getParam(param)->value() + "\n").c_str());
    }
    if (!request->hasParam("elem", true))
      return;
    if (!request->hasParam("move", true))
      return;
    
    ReorderElement(std::atoi(request->getParam("elem", true)->value().c_str()), std::atoi(request->getParam("move", true)->value().c_str()));
    ReloadTestData();
  });

  server.on("/set_date", HTTP_POST, [this](AsyncWebServerRequest * request){
    LOG_DEBUG("/set_date");
    for (size_t param = 0; param < request->params(); param++)
      LOG_DEBUG((request->getParam(param)->name() + " : " + request->getParam(param)->value() + "\n").c_str());

    if (!(request->hasParam("year", true) && request->hasParam("month", true) && request->hasParam("day", true) && request->hasParam("weekday", true)))
      return;
    
    rtc_handler->setDate(std::atoi(request->getParam("day", true)->value().c_str()),
      std::atoi(request->getParam("weekday", true)->value().c_str()),
      std::atoi(request->getParam("month", true)->value().c_str()),
      std::atoi(request->getParam("year", true)->value().c_str()));
  });

  server.on("/set_time", HTTP_POST, [this](AsyncWebServerRequest * request){
    LOG_DEBUG("/set_time");
    for (size_t param = 0; param < request->params(); param++)
      LOG_DEBUG((request->getParam(param)->name() + " : " + request->getParam(param)->value() + "\n").c_str());

    if (!(request->hasParam("hours", true) && request->hasParam("minutes", true) && request->hasParam("seconds", true)))
      return;
    
    rtc_handler->setTime(std::atoi(request->getParam("hours", true)->value().c_str()),
      std::atoi(request->getParam("minutes", true)->value().c_str()),
      std::atoi(request->getParam("seconds", true)->value().c_str()));
  });

  server.on("/update", HTTP_POST, std::bind(&ConfigServer::ProcessUpdateRequest, this, std::placeholders::_1),
      std::bind(&ConfigServer::ProcessUpdateUpload, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6));

  // Create the element selection and data strings for the ui_settings.html template
  for (size_t ui_code = 1; ui_code < kMaxUiElementCode; ui_code++) {
    uint8_t data[] = {static_cast<uint8_t>(ui_code)};
    UiElement* elem = UiElement::Factory(data, 1);

    // This means the code was not valid.
    if (String(elem->Name()) == String("Empty")) {
      delete elem;
      continue;
    }

    ui_elem_select += "<option value=\"";
    ui_elem_select += ui_code;
    ui_elem_select += "\">";
    ui_elem_select += elem->Name();
    ui_elem_select += "</option>\n";

    ui_elem_data += (ui_code > 1? "," : "");
    ui_elem_data += "\"";
    ui_elem_data += ui_code;
    ui_elem_data += "\": [";

    std::vector<String> name_list = elem->ArgNames();
    std::vector<ArgType> arg_list = elem->ArgList();
    for (size_t arg = 0; arg < arg_list.size(); arg++) {
      ui_elem_data += String(arg > 0? "," : "") + "[\"" + String(static_cast<uint8_t>(arg_list.at(arg))) + "\",\"";

      // Fill in the custom name where applicable, use the default name otherwise
      if (arg < name_list.size())
        ui_elem_data += name_list.at(arg);
      else
        ui_elem_data += kArgTypeNames[static_cast<size_t>(arg_list.at(arg))];

      ui_elem_data += "\"]";
    }

    ui_elem_data += "]";

    delete elem;
  }

  // Create the data type select string for the ui_settings.html template
  for (uint8_t data_type = 0; data_type < static_cast<uint8_t>(DataType::kLastValue); data_type++) {
    ui_data_type_select += "<option value=\"" + String(data_type) + "\">" + kDataTypeNames[data_type] + "</option>\n";
  }

  // If there are saved UI preferences, load them.
  std::vector<uint8_t> csv_data;
  if (file_handler->FileSize(Utils::getUiScreenFileName(ui_screen))) {
    csv_data = file_handler->ReadCsvBytes(Utils::getUiScreenFileName(ui_screen));
  } else {  // If not, use the "factory" settings
    csv_data = std::vector<uint8_t>(kUiDefaultPreferences, kUiDefaultPreferences + kUiDefaultPreferencesLength);
  }

  // Now separate the bytes into their respective elements
  for (size_t counter = 0; counter < csv_data.size();) {
    UiElement *elem = UiElement::Factory(csv_data.data() + counter, csv_data.size() - counter);
    // Construct an array of all the bytes representing this element, and place it into the larger array of elements
    test_ui_data.emplace_back(csv_data.data() + counter, csv_data.data() + counter + elem->DataSize());

    counter += elem->DataSize(); // Add the data size to the running total

    delete elem;  // Clean up memory
  }
}

ConfigServer::~ConfigServer() {
  if (started)
    Stop();
}

void ConfigServer::Start() {
  started = true;
  WiFi.softAP(kDefaultServerSSID);
  LOG_DEBUG(WiFi.softAPIP().toString().c_str());
  ui_handler->ChangeScreen(UiScreen::kConfig);

  if (MDNS.begin(kDefaultURL))
    ui_handler->ShowMessage((String(kDefaultURL) + ".local").c_str(), 60);
  else
    ui_handler->ShowMessage(WiFi.softAPIP().toString().c_str(), 60);

  ui_handler->Update(&test_process_data);
  server.begin();
}

void ConfigServer::Stop() {
  started = false;
  server.end();
  WiFi.disconnect();
  MDNS.end();
}

bool ConfigServer::isStarted() {
  return started;
}

// Processing the ui_settings.html template page and replacing placeholders with real data
String ConfigServer::ProcessUiPage(const String& placeholder) {
  String out = "";

  if (placeholder == "UI_ELEM_DATA") {
    // Replace with UI element code and args
    // format: "1": [["0":"argument name"], ["1":"other argument name"]],
    out = ui_elem_data;
  } else if (placeholder == "UI_ELEM_SELECT") {
    // Replace with UI element code and name
    // format: <option value="1">Background</option>
    out = ui_elem_select;
  } else if (placeholder == "UI_DATA_TYPE_SELECT") {
    // Replace with Data Type code and name
    // format: <option value="1">Time</option>
    out = ui_data_type_select;
  } else if (placeholder == "UI_DRAW_QUEUE_TABLE") {
    // Replace with table rows representing each UI Element in the current draw list
    // format: <tr><td>Background</td></tr>
    size_t elem_count = 0;
    for (UiElement* draw_elem : *ui_handler->getDrawList()) {
      out += "<tr><td>" + String(draw_elem->Name())
        + "</td><td><button type=\"button\" onclick=\"deleteElement(this)\">Delete</button></td><td><button onclick=\"moveElemUp(this)\">↑</button></td><td><button onclick=\"moveElemDown(this)\">↓</button></td></tr>\n";
      elem_count++;
    }
  } else if (placeholder == "UI_DATA_TABLE") {
    // Replace with table rows representing each type of data that is being displayed by the UI
    // format: <tr><td>Speed</td></tr>
    for (size_t data_type = 0; data_type < (size_t)DataType::kLastValue; data_type++)
      if (test_data_types[data_type])
        out += "<tr id=\"" + String(data_type) + "\"><td>" + String(kDataTypeNames[static_cast<size_t>(data_type)])
          + "</td><td><input type=\"number\" onchange=\"updateData(this)\"></td></tr>\n";
  }

  Serial.println(out);

  return out;
}

void ConfigServer::ProcessNewElementRequest(AsyncWebServerRequest *request) {
  LOG_DEBUG("/new_element");

  bool has_error = false;
  uint8_t elem_type;

  if (request->hasParam("ui_element", true))
    elem_type = std::atoi(request->getParam("ui_element", true)->value().c_str());
  else
    has_error = true;

  for (size_t param = 0; param < request->params(); param++) {
    LOG_DEBUG((request->getParam(param)->name() + " : " + request->getParam(param)->value() + "\n").c_str());
  }

  UiElement* elem = UiElement::Factory(&elem_type, 1);
  std::vector<ArgType> args = elem->ArgList();
  std::vector<uint8_t> data = {elem_type};

  delete elem;  // Clean up to avoid memory leaks

  for (size_t arg = 0; arg < args.size(); arg++) {
    switch (args.at(arg)) {
      case ArgType::kDataType: {
        if (request->hasParam((String("data_arg") + arg).c_str(), true)) {
          data.emplace_back(std::atoi(request->getParam((String("data_arg") + arg).c_str(), true)->value().c_str()));
          test_data_types[std::atoi(request->getParam((String("data_arg") + arg).c_str(), true)->value().c_str())] = true;  // Mark this datatype as being used
        } else {
          has_error = true;
          LOG_DEBUG("Has Error: %d");
        }
        break; }
      case ArgType::kColour: {
        const char* param_name = (String("colour_arg") + arg).c_str();
        LOG_DEBUG_ARGS("Looking for arg: %s\n", param_name);

        if (request->hasParam(param_name, true)) {
          if (request->getParam(param_name, true)->value() == String((int)ColourType::kConstant)) {  // Solid Colour
            data.emplace_back((uint8_t)ColourType::kConstant);

            const char* colour_type = (String("solid_colour_arg") + arg).c_str();

            if (request->hasParam(colour_type, true)) {
              std::vector<uint8_t> parsed_colour = ParseColour(request->getParam(colour_type, true)->value());
              if (parsed_colour.size() < 3) {
                has_error = true;
                LOG_DEBUG("Has Error");
              } else {
                data.insert(data.end(), parsed_colour.begin(), parsed_colour.end());
              }
            } else {
              has_error = true;
              LOG_DEBUG("Has Error");
            }
          } else if (request->getParam(param_name, true)->value() == String((int)ColourType::kDynamicBetweenValues)) { // Dynamic Colour
            data.emplace_back((uint8_t)ColourType::kDynamicBetweenValues);

            if (request->hasParam((String("colour_data_type") + arg).c_str(), true)
                && request->hasParam((String("arg_low_data") + arg).c_str(), true)
                && request->hasParam((String("arg_low_colour") + arg).c_str(), true)
                && request->hasParam((String("arg_high_data") + arg).c_str(), true)
                && request->hasParam((String("arg_high_colour") + arg).c_str(), true)) {
              data.emplace_back(std::atoi(request->getParam((String("colour_data_type") + arg).c_str(), true)->value().c_str()));
              test_data_types[std::atoi(request->getParam((String("colour_data_type") + arg).c_str(), true)->value().c_str())] = true;  // Mark this datatype as being used

              data.emplace_back(std::atoi(request->getParam((String("arg_low_data") + arg).c_str(), true)->value().c_str()));
              data.emplace_back(std::atoi(request->getParam((String("arg_high_data") + arg).c_str(), true)->value().c_str()));

              std::vector<uint8_t> parsed_low_colour = ParseColour(request->getParam((String("arg_low_colour") + arg).c_str(), true)->value());
              std::vector<uint8_t> parsed_high_colour = ParseColour(request->getParam((String("arg_high_colour") + arg).c_str(), true)->value());

              if (parsed_low_colour.size() < 3 || parsed_high_colour.size() < 3) {
                has_error = true;
                LOG_DEBUG("Has Error");
              }

              data.insert(data.end(), parsed_low_colour.begin(), parsed_low_colour.end());
              data.insert(data.end(), parsed_high_colour.begin(), parsed_high_colour.end());
            }
          }
        } else {
          has_error = true;
          LOG_DEBUG("Has Error");
        }
        break; }
      case ArgType::kConstant: {
        if (request->hasParam((String("constant_arg") + arg).c_str(), true)) {
          data.emplace_back(std::atoi(request->getParam((String("constant_arg") + arg).c_str(), true)->value().c_str()));
        } else {
          has_error = true;
          LOG_DEBUG("Has Error");
        }
        break; }
    }
  }

  if (!has_error) {
    LOG_DEBUG("Inserting new element");

    LOG_DEBUG("Data: ");
    for (uint8_t byte : data)
      LOG_DEBUG_ARGS("%d, ", byte);

    // Appending this new element to the test data.
    test_ui_data.emplace_back(data);
    ReloadTestData();
  } else {
    LOG_DEBUG("Element has error");
    LOG_DEBUG("Data: ");
    for (uint8_t byte : data)
      LOG_DEBUG_ARGS("%d, ", byte);

    // TODO: Handle notifying user of errors here
  }

  // Resend the updated UI page
  request->send(SPIFFS, "/ui_settings.html", "text/html", false, std::bind(&ConfigServer::ProcessUiPage, this, std::placeholders::_1));
}

void ConfigServer::ProcessUpdateRequest(AsyncWebServerRequest *request) {
  bool shouldReboot = !Update.hasError();
  AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", shouldReboot?"OK":"FAIL");
  response->addHeader("Connection", "close");
  request->send(response);
}

void ConfigServer::ProcessUpdateUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
  if(!index){
    LOG_DEBUG_ARGS("Update Start: %s", filename.c_str());
    if(!Update.begin((ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000)){
      Update.printError(Serial);
    }
  }
  if(!Update.hasError()){
    if(Update.write(data, len) != len){
      Update.printError(Serial);
    }
  }
  if(final){
    if(Update.end(true)){
      LOG_DEBUG_ARGS("Update Success: %uB", index+len);
      ESP.restart();
    } else {
      Update.printError(Serial);
    }
  }
}

std::vector<uint8_t> ConfigServer::ParseColour(String colour) {
  std::vector<uint8_t> out;
  out.reserve(3); // Always 3 bytes

  // Colour is represented by a string like "#ff00bb" where each pair of digits is a number representing
  // red (0xff), green (0x00), and blue (0xbb) in hexadecimal

  if (colour.length() < 7) {
    return out;
  }
  
  // Converts from string in base 16 to unsigned long, and from there to unsigned byte.
  out.emplace_back(std::strtoul(colour.substring(1, 3).c_str(), NULL, 16)); // Red
  out.emplace_back(std::strtoul(colour.substring(3, 5).c_str(), NULL, 16)); // Green
  out.emplace_back(std::strtoul(colour.substring(5).c_str(), NULL, 16)); // Blue

  return out;
}

void ConfigServer::RemoveElement(size_t index) {
  if (index >= test_ui_data.size()) // Avoid out of range elements
    return;

  test_ui_data.erase(test_ui_data.begin() + index);
}

void ConfigServer::ReorderElement(size_t index, int move) {
  if (index >= test_ui_data.size()) // Avoid out of range elements
    return;

  if (move > 0 && index == (test_ui_data.size() - 1)) // If the last element is being moved forward, ignore it
    return;
  
  test_ui_data.insert(test_ui_data.begin() + (index + move + (move > 0? 1 : 0)), test_ui_data.at(index));
  test_ui_data.erase(test_ui_data.begin() + (index + (move < 0? 1 : 0)));
}

void ConfigServer::ReloadTestData() {
  std::vector<uint8_t> new_data;
  new_data.reserve(test_ui_data.size() * 2);  // Just a guess at the total size to minimise the reallocations needed

  for (std::vector<uint8_t> byte_vec : test_ui_data)  // Combine all the vectors into one
    new_data.insert(new_data.end(), byte_vec.begin(), byte_vec.end());

  ui_handler->LoadFromData(new_data.data(), new_data.size());
  ui_handler->Update(&test_process_data);
}

}
