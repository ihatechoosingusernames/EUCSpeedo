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

    void SaveSettings();

  private:
    uint8_t general_settings[static_cast<size_t>(GeneralSetting::kLastValue)];
    std::vector<std::array<uint8_t, static_cast<size_t>(ScreenSetting::kLastValue)>> screen_settings;

    FileHandler* file_handler;
};

}

#endif