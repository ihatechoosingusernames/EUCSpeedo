#include "uihandler.h"

#include "constants.h"

namespace euc {

UiHandler::UiHandler(FileHandler* file_handler) {
  LoadFromPreferences(file_handler);
}

UiHandler::~UiHandler() {
  // Clean up draw list when object destructed
  for (UiElement* element : draw_list) {
    delete element;
  }
}

void UiHandler::Update(ProcessData* data) {
  for (UiElement* element : draw_list) {
    element->Draw(data);
  }
}

void UiHandler::LoadFromPreferences(FileHandler* file_handler) {
  size_t data_len = file_handler->FileSize(kUiPreferences);

  // If there are saved UI preferences, load them.
  if (data_len) {
    uint8_t data[data_len];
    file_handler->ReadFile(kUiPreferences, data, &data_len);
    LoadFromData(data, data_len);
    return;
  }

  // If there are no saved prefs, use the defaults
  data_len = kUiDefaultPreferencesLength;
  uint8_t data[data_len];

  std::copy(kUiDefaultPreferences, kUiDefaultPreferences+data_len, data);

  LoadFromData(data, data_len);
}

void UiHandler::LoadFromData(uint8_t data[], size_t data_len) {
  for (size_t data_used = 0; data_used < data_len;) {
    // The UiElement factory makes the appropriate UI element for the given data
    UiElement* element = UiElement::Factory(data + data_used, data_len - data_used);

    // The created element tells us how much data it has used
    data_used += element->DataSize();

    Serial.printf("Created element of code %X and size %d\n", data[0], element->DataSize());

    // The elements are created in draw order; so the later in the list, the later they should be drawn
    draw_list.emplace_back(element);
  }
}

}