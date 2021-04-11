#include "settings.h"
#include "logging.h"

namespace euc {

Settings::Settings(FileHandler* file_handler) : file_handler(file_handler) {
  std::vector<uint8_t> csv_bytes = file_handler->ReadCsvBytes(kGeneralSettingsFile);

  size_t counter = 0, screen_value_at = 0;
  for (uint8_t byte : csv_bytes) {
    if (counter < static_cast<size_t>(GeneralSetting::kLastValue)) {  // First bytes are general settings
      general_settings[counter] = byte;
    } else if (!(counter - static_cast<size_t>(GeneralSetting::kLastValue)) % static_cast<size_t>(ScreenSetting::kLastValue) && !byte) { // Then each screen's settings with a delimiter
      screen_value_at = counter;
      screen_settings.emplace_back(); // Create a new array of screen settings
    } else if ((counter - screen_value_at - 1) < static_cast<size_t>(ScreenSetting::kLastValue)) {  // Add the screen settings
      screen_settings.at(screen_settings.size() - 1)[(counter - screen_value_at - 1)] = byte;
      LOG_DEBUG_ARGS("Screen setting %d is %d", (counter - screen_value_at - 1), byte);
    }

    counter++;
  }
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
  std::array<uint8_t, static_cast<size_t>(ScreenSetting::kLastValue)> new_screen;
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
  
  std::array<uint8_t, static_cast<size_t>(ScreenSetting::kLastValue)> moving_screen = screen_settings[screen];
  screen_settings[screen] = screen_settings[other_screen];
  screen_settings[other_screen] = moving_screen;
}

void Settings::SaveSettings() {
  String csv = "";

  for (size_t counter = 0; counter < static_cast<size_t>(GeneralSetting::kLastValue); counter++)
    csv += String(general_settings[counter]) + ", ";
  
  for (std::array<uint8_t, static_cast<size_t>(ScreenSetting::kLastValue)> settings : screen_settings)
    for (size_t counter = 0; counter < static_cast<size_t>(ScreenSetting::kLastValue); counter++)
      if (counter)
        csv += String(settings[counter]) + ", ";
      else
        csv += String("0, ") + settings[counter] + ", ";  // Adding a delimiter between screens

  LOG_DEBUG_ARGS("Saving settings as %s", csv.c_str());

  file_handler->WriteFile(kGeneralSettingsFile, csv);
}

}