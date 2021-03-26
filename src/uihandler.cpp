#include "uihandler.h"

#include "constants.h"

namespace euc {

UiHandler::UiHandler(FileHandler* file_handler, UiScreen start_screen) : sprite(&screen), ui_screen(start_screen), file_handler(file_handler) {
  LoadFromFile(file_handler);
  screen.init();
}

UiHandler::~UiHandler() {
  // Clean up draw list when object destructed
  ClearDrawList();
}

void UiHandler::ChangeScreen(UiScreen new_ui_screen) {
  ui_screen = new_ui_screen;
  LoadFromFile(file_handler);
}

void UiHandler::Update(ProcessData* data) {
  sprite.createSprite(screen.width(), screen.height()); // Create a sprite the size of the screen

  for (UiElement* element : draw_list) {
    element->Draw(data, &sprite);
  }

  // sprite.pushSprite(0, 0);  // Push the sprite to the screen TODO: Uncomment this when actually running a screen
}

void UiHandler::LoadFromData(uint8_t data[], size_t data_len) {
  ClearDrawList();  // Clear draw list so all elements are replaced
  
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

UiScreen UiHandler::getCurrentScreen() { return ui_screen; }

std::vector<UiElement*>* UiHandler::getDrawList() { return &draw_list; }

void UiHandler::LoadFromFile(FileHandler* file_handler) {
  printf("Loading screen %d\n", static_cast<uint8_t>(ui_screen));

  // Building the screen's filename
  String ui_screen_prefs = kUiScreenFilePrefix + String(static_cast<uint8_t>(ui_screen)) + "." + kUiScreenFileType;

  // If there are saved UI preferences, load them.
  if (file_handler->FileSize(ui_screen_prefs.c_str())) {
    std::vector<uint8_t> from_csv = file_handler->ReadCsvBytes(ui_screen_prefs.c_str());

    LoadFromData(from_csv.data(), from_csv.size());
  }

  printf("No data to load from file, loading factory config\n");

  // If there are no saved prefs, use the defaults
  size_t data_len = kUiDefaultPreferencesLength;
  uint8_t data[data_len];

  std::copy(kUiDefaultPreferences, kUiDefaultPreferences+data_len, data);

  LoadFromData(data, data_len);
}

void UiHandler::ClearDrawList() {
  for (UiElement* element : draw_list) {
    delete element;
  }

  draw_list.clear();  // Reset draw list
}

}