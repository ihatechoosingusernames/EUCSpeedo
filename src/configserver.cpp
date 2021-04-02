#include "configserver.h"

#include <functional>
#include "SPIFFS.h"

#include "constants.h"
#include "uielement.h"
#include "utils.h"

namespace euc {

ConfigServer::ConfigServer(UiHandler* arg_ui_handler, FileHandler* files, RtcHandler* rtc) : server(kDefaultServerPort),
    ui_handler(arg_ui_handler), file_handler(files), rtc_handler(rtc) {
  server.on("/", HTTP_GET, [this](AsyncWebServerRequest *request){
    printf("/\n");
    request->send(SPIFFS, "/ui_settings.html", "text/html", false, std::bind(&ConfigServer::ProcessUiPage, this, std::placeholders::_1));
  });

  server.on("/general_settings", HTTP_GET, [this](AsyncWebServerRequest *request){
    printf("/general_settings\n");
    request->send(SPIFFS, "/general_settings.html", "text/html", false);
  });

  server.on("/style.css", HTTP_GET, [this](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/style.css", "text/css");
  });

  server.on("/new_element", HTTP_POST, std::bind(&ConfigServer::ProcessNewElementRequest, this, std::placeholders::_1));

  server.on("/save_changes", HTTP_POST, [this](AsyncWebServerRequest *request){
    printf("/save_changes\n");
    // Convert test data to a CSV string and store it in the ui data file
    String ui_data_string;
    for (std::vector<uint8_t> byte_vec : test_ui_data) {
      for (uint8_t byte : byte_vec)
        ui_data_string += String(byte) + ", ";
      ui_data_string += "\n";
    }

    file_handler->WriteFile(Utils::getUiScreenFileName(ui_screen), ui_data_string.c_str());
  });

  server.on("/remove_element", HTTP_DELETE, [this](AsyncWebServerRequest *request){
    printf("/remove_element\n");
    if(request->hasParam("id")) {
      RemoveElement(std::atoi(request->getParam("id")->value().c_str()));
      ReloadTestData();
    }
  });

  server.on("/update_data", HTTP_POST, [this](AsyncWebServerRequest * request){
    printf("/update_data\n");
    if (!request->hasParam("id", true))
      return;
    if (!request->hasParam("data", true))
      return;

    printf(("Updating data type " + request->getParam("id", true)->value() + " with data " + request->getParam("data", true)->value() + "\n").c_str());

    // Get data type as a string, then convert to String -> const char* -> int -> DataType. Very efficient.
    DataType data = static_cast<DataType>(std::atoi(request->getParam("id", true)->value().c_str()));
    double val = std::atof(request->getParam("data", true)->value().c_str());

    test_process_data.Update(data, val);
    ui_handler->Update(&test_process_data);
  });

  server.on("/update_elem_order", HTTP_POST, [this](AsyncWebServerRequest * request){
    printf("/update_elem_order\n");
    for (size_t param = 0; param < request->params(); param++) {
      printf((request->getParam(param)->name() + " : " + request->getParam(param)->value() + "\n").c_str());
    }
    if (!request->hasParam("elem", true))
      return;
    if (!request->hasParam("move", true))
      return;
    
    ReorderElement(std::atoi(request->getParam("elem", true)->value().c_str()), std::atoi(request->getParam("move", true)->value().c_str()));
    ReloadTestData();
  });

  server.on("/set_date", HTTP_POST, [this](AsyncWebServerRequest * request){
    printf("/set_date\n");
    for (size_t param = 0; param < request->params(); param++)
      printf((request->getParam(param)->name() + " : " + request->getParam(param)->value() + "\n").c_str());

    if (!(request->hasParam("year", true) && request->hasParam("month", true) && request->hasParam("day", true) && request->hasParam("weekday", true)))
      return;
    
    rtc_handler->setDate(std::atoi(request->getParam("day", true)->value().c_str()),
      std::atoi(request->getParam("weekday", true)->value().c_str()),
      std::atoi(request->getParam("month", true)->value().c_str()),
      std::atoi(request->getParam("year", true)->value().c_str()));
  });

  server.on("/set_time", HTTP_POST, [this](AsyncWebServerRequest * request){
    printf("/set_time\n");
    for (size_t param = 0; param < request->params(); param++)
      printf((request->getParam(param)->name() + " : " + request->getParam(param)->value() + "\n").c_str());

    if (!(request->hasParam("hours", true) && request->hasParam("minutes", true) && request->hasParam("seconds", true)))
      return;
    
    rtc_handler->setTime(std::atoi(request->getParam("hours", true)->value().c_str()),
      std::atoi(request->getParam("minutes", true)->value().c_str()),
      std::atoi(request->getParam("seconds", true)->value().c_str()));
  });

  // Create the element selection and data strings for the ui_settings.html template
  for (size_t ui_code = 1; ui_code < kMaxUiElementCode; ui_code++) {
    uint8_t data[] = {static_cast<uint8_t>(ui_code)};
    UiElement* elem = UiElement::Factory(data, 1);

    // This means the code was not valid.
    if (String(elem->Name()) == String("Empty")) {
      delete elem;
      continue;
    }

    ui_elem_select += "<option value=\"" + String(ui_code) + "\">" + elem->Name() + "</option>\n";

    ui_elem_data += String(ui_code > 1? "," : "") + "\"" + String(ui_code) + "\": [";

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
  String ui_screen_prefs = Utils::getUiScreenFileName(ui_screen);

  std::vector<uint8_t> csv_data;
  if (file_handler->FileSize(ui_screen_prefs.c_str())) {
    csv_data = file_handler->ReadCsvBytes(ui_screen_prefs.c_str());
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
  printf("/new_element\n");

  bool has_error = false;
  uint8_t elem_type;

  if (request->hasParam("ui_element", true))
    elem_type = std::atoi(request->getParam("ui_element", true)->value().c_str());
  else
    has_error = true;

  for (size_t param = 0; param < request->params(); param++) {
    printf((request->getParam(param)->name() + " : " + request->getParam(param)->value() + "\n").c_str());
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
          printf("Has Error: %d\n", __LINE__);
        }
        break; }
      case ArgType::kColour: {
        const char* param_name = (String("colour_arg") + arg).c_str();
        printf("Looking for arg: %s\n", param_name);

        if (request->hasParam(param_name, true)) {
          if (request->getParam(param_name, true)->value() == String((int)ColourType::kConstant)) {  // Solid Colour
            data.emplace_back((uint8_t)ColourType::kConstant);

            const char* colour_type = (String("solid_colour_arg") + arg).c_str();

            if (request->hasParam(colour_type, true)) {
              std::vector<uint8_t> parsed_colour = ParseColour(request->getParam(colour_type, true)->value());
              if (parsed_colour.size() < 3) {
                has_error = true;
                printf("Has Error: %d\n", __LINE__);
              } else {
                data.insert(data.end(), parsed_colour.begin(), parsed_colour.end());
              }
            } else {
              has_error = true;
              printf("Has Error: %d\n", __LINE__);
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
                printf("Has Error: %d\n", __LINE__);
              }

              data.insert(data.end(), parsed_low_colour.begin(), parsed_low_colour.end());
              data.insert(data.end(), parsed_high_colour.begin(), parsed_high_colour.end());
            }
          }
        } else {
          has_error = true;
          printf("Has Error: %d\n", __LINE__);
        }
        break; }
      case ArgType::kConstant: {
        if (request->hasParam((String("constant_arg") + arg).c_str(), true)) {
          data.emplace_back(std::atoi(request->getParam((String("constant_arg") + arg).c_str(), true)->value().c_str()));
        } else {
          has_error = true;
          printf("Has Error: %d\n", __LINE__);
        }
        break; }
    }
  }

  if (!has_error) {
    printf("Inserting new element\n");

    printf("Data: ");
    for (uint8_t byte : data)
      printf("%d, ", (int)byte);

    printf("\n");

    // Appending this new element to the test data.
    test_ui_data.emplace_back(data);
    ReloadTestData();
  } else {
    printf("Element has error\n");
    printf("Data: ");
    for (uint8_t byte : data)
      printf("%d, ", (int)byte);

    printf("\n");
    // TODO: Handle notifying user of errors here
  }

  // Resend the updated UI page
  request->send(SPIFFS, "/ui_settings.html", "text/html", false, std::bind(&ConfigServer::ProcessUiPage, this, std::placeholders::_1));
}

std::vector<uint8_t> ConfigServer::ParseColour(String colour) {
  std::vector<uint8_t> out;
  out.reserve(3); // Always 3 bytes

  // Colour is represented by a string like "#ff00bb" where each pair of digits is a number representing
  // red (0xff), green (0x00), and blue (0xbb) in hexadecimal

  if (colour.length() < 7) {
    return out;
  }
  
  // Converts from string to unsigned long in base 16, and from there to unsigned byte.
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
