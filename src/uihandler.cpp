#include "uihandler.h"

#include "constants.h"
#include "hwconfig.h"
#include "utils.h"
#include "logging.h"

namespace euc {

UiHandler::UiHandler(FileHandler* file_handler, UiScreen start_screen) : ui_screen(start_screen), file_handler(file_handler) {
  LoadFromFile(file_handler);
  screen.init();
  screen.setPivot(screen.width() / 2, screen.height() / 2);
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
  TFT_eSprite sprite = TFT_eSprite(&screen); // Sprite object acts as a screen buffer to allow fast screen changes
  sprite.createSprite(TFT_HEIGHT, TFT_WIDTH); // Screen is in portrait, we want to draw in landscape

  for (UiElement* element : draw_list) {
    element->Draw(data, &sprite);
  }

  if (message_timeout > millis()) {
    sprite.loadFont(kSmallTextFont);
    sprite.setTextWrap(true, true);
    sprite.setCursor(2, 2);
    sprite.setTextColor(TFT_WHITE, TFT_BLACK);
    sprite.println(message);
    sprite.unloadFont();
  }

  sprite.setPivot(screen.height() / 2, (screen.width() / 2) + 1); // Adding 1 to the pivot point to correct for division error
  sprite.pushRotated(270);  // Push the sprite to the screen, rotating so it's 'the right way up'
  sprite.deleteSprite();
}

void UiHandler::LoadFromData(uint8_t data[], size_t data_len) {
  ClearDrawList();  // Clear draw list so all elements are replaced
  
  for (size_t data_used = 0; data_used < data_len;) {
    // The UiElement factory makes the appropriate UI element for the given data
    UiElement* element = UiElement::Factory(data + data_used, data_len - data_used);

    LOG_DEBUG_ARGS("Created element of code %X and size %d\n", data[data_used], element->DataSize());

    // The created element tells us how much data it has used
    data_used += element->DataSize();

    // The elements are created in draw order; so the later in the list, the later they should be drawn
    draw_list.emplace_back(element);
  }
}

void UiHandler::Sleep() {
  screen.writecommand(ST7735_SLPIN);
  screen.writecommand(ST7735_DISPOFF);
  esp_sleep_enable_ext0_wakeup(static_cast<gpio_num_t>(PIN_223B_Q), HIGH);  // Wake on button press
  esp_deep_sleep_start();
}

void UiHandler::ShowMessage(const char* in_message, double time) {
  message = in_message;
  message_timeout = millis() + (time * 1000);
}

UiScreen UiHandler::getCurrentScreen() { return ui_screen; }

std::vector<UiElement*>* UiHandler::getDrawList() { return &draw_list; }

void UiHandler::LoadFromFile(FileHandler* file_handler) {
  LOG_DEBUG_ARGS("Loading screen %d\n", static_cast<uint8_t>(ui_screen));

  // If there are saved UI preferences, load them.
  if (file_handler->FileSize(Utils::getUiScreenFileName(ui_screen))) {
    std::vector<uint8_t> from_csv = file_handler->ReadCsvBytes(Utils::getUiScreenFileName(ui_screen));

    LoadFromData(from_csv.data(), from_csv.size());
    return;
  }

  LOG_DEBUG("No data to load from file, loading factory config\n");

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