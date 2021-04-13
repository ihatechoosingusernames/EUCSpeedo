#ifndef SETTINGS_H_
#define SETTINGS_H_

#include "constants.h"
#include "filehandler.h"

namespace euc {

class Settings {
  public:
    Settings(FileHandler* file_handler);

    uint8_t getSetting(GeneralSetting setting);
    void setSetting(GeneralSetting setting, uint8_t value);

    uint8_t getScreenSetting(uint8_t screen, ScreenSetting setting);
    void setScreenSetting(uint8_t screen, ScreenSetting setting, uint8_t value);
    uint8_t getNumScreens();
    
    void AddScreen();
    void RemoveScreen(uint8_t screen);
    void SwitchScreens(uint8_t screen, uint8_t other_screen);

    void SaveSettings();

  private:
    static const uint8_t screen_settings_size = static_cast<uint8_t>(ScreenSetting::kLastValue);
    static const uint8_t general_settings_size = static_cast<uint8_t>(GeneralSetting::kLastValue);

    uint8_t general_settings[static_cast<size_t>(GeneralSetting::kLastValue)];
    std::vector<std::array<uint8_t, screen_settings_size>> screen_settings;

    FileHandler* file_handler;
};

}

#endif