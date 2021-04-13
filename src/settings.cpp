#include "settings.h"
#include "logging.h"

namespace euc {

Settings::Settings(FileHandler* file_handler) : file_handler(file_handler) {
  std::vector<uint8_t> csv_bytes = file_handler->ReadCsvBytes(kGeneralSettingsFile);

  size_t counter = 0, screen_value_at = 0;
  for (uint8_t byte : csv_bytes) {
    if (counter < general_settings_size) {  // First bytes are general settings
      general_settings[counter] = byte;
    } else if (!((counter - general_settings_size) % (screen_settings_size + 1)) && !byte) { // Then each screen's settings with a delimiter
      screen_value_at = counter;
      screen_settings.emplace_back(); // Create a new array of screen settings
    } else if ((counter - screen_value_at - 1) < screen_settings_size) {  // Add the screen settings
      screen_settings.at(screen_settings.size() - 1)[counter - screen_value_at - 1] = byte;
      LOG_DEBUG_ARGS("Screen setting %d is %d", (counter - screen_value_at - 1), byte);
    } else {
      LOG_DEBUG_ARGS("Counter at %d, counter  - screen_value_at %d, screen_settings_size %d", counter, (counter - screen_value_at - 1), screen_settings_size)
    }

    counter++;
  }

  LOG_DEBUG_ARGS("Number of screens: %d", getNumScreens());
}

uint8_t Settings::getSetting(GeneralSetting setting) {
  return general_settings[static_cast<size_t>(setting)];
}

void Settings::setSetting(GeneralSetting setting, uint8_t value) {
  general_settings[static_cast<size_t>(setting)] = value;
}

uint8_t Settings::getScreenSetting(uint8_t screen, ScreenSetting setting) {
  if (screen >= screen_settings.size())
    return 0;
  
  return screen_settings.at(screen)[static_cast<size_t>(setting)];
}

void Settings::setScreenSetting(uint8_t screen, ScreenSetting setting, uint8_t value) {
  if (screen >= screen_settings.size())
    return;
  
  screen_settings.at(screen)[static_cast<size_t>(setting)] = value;
}

uint8_t Settings::getNumScreens() {
  return screen_settings.size();
}

void Settings::AddScreen() {
  std::array<uint8_t, screen_settings_size> new_screen;
  std::fill(new_screen.begin(), new_screen.end(), 0);
  screen_settings.emplace_back(new_screen);
}

void Settings::RemoveScreen(uint8_t screen) {
  if (screen < screen_settings.size())
    screen_settings.erase(screen_settings.begin() + screen);
}

void Settings::SwitchScreens(uint8_t screen, uint8_t other_screen) {
  if (std::max(screen, other_screen) >= screen_settings.size())
    return;
  
  std::array<uint8_t, screen_settings_size> moving_screen = screen_settings[screen];
  screen_settings[screen] = screen_settings[other_screen];
  screen_settings[other_screen] = moving_screen;
}

void Settings::SaveSettings() {
  String csv = "";

  for (size_t counter = 0; counter < general_settings_size; counter++)
    csv += String(general_settings[counter]) + ", ";
  
  for (std::array<uint8_t, screen_settings_size> settings : screen_settings)
    for (size_t counter = 0; counter < screen_settings_size; counter++)
      if (counter)
        csv += String(settings[counter]) + ", ";
      else
        csv += String("0, ") + settings[counter] + ", ";  // Adding a delimiter between screens

  LOG_DEBUG_ARGS("Saving settings as %s", csv.c_str());

  file_handler->WriteFile(kGeneralSettingsFile, csv);
}

}