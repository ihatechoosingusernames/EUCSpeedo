#include "uihandler.h"

#include <Preferences.h>

#include "constants.h"

namespace euc {

UiHandler::UiHandler() {
  LoadFromPreferences();
}

void UiHandler::Update() {
  for (UiElement element : draw_list) {
    element.Draw();
  }
}

void LoadFromPreferences() {
  Preferences preferences;
  preferences.begin(kEucPreferences, false);

  size_t data_len = preferences.getBytesLength(kUiPreferences);

  if (data_len) {
    uint8_t data[data_len];
    preferences.getBytes(kUiPreferences, data, data_len);

  } else {  // If there are no preferences saved, copy in the default prefs
    data_len = kUiDefaultPreferencesLength;
    uint8_t data[data_len];

    std::copy(kUiDefaultPreferences, kUiDefaultPreferences+data_len, data);
  }
}

void UiHandler::LoadFromData(uint8_t data[], size_t data_len) {
  for (size_t data_used = 0; data_used < data_len;) {
    // The UiElement factory makes the appropriate UI element for the given data
    UiElement element = UiElement::Factory(data, data_len);

    // The created element tells us how much data it has used
    data_used += element.DataSize();

    // The elements are created in draw order; so the later in the list, the later they should be drawn
    draw_list.emplace_back(element);
  }
}

}