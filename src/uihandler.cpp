#include "uihandler.h"

#include "constants.h"

namespace euc {

UiHandler::UiHandler() {
  LoadFromPreferences();
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

void UiHandler::LoadFromPreferences() {
  size_t data_len = kUiDefaultPreferencesLength;
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