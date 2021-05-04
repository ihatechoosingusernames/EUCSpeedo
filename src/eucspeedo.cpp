#include "eucspeedo.h"

#include <functional>
#include <memory>

#include "eucs/includes.h"
#include "constants.h"
#include "buttonhandler.h"
#include "logging.h"

namespace euc {

EucSpeedo::EucSpeedo() : button_handler(ButtonHandler::getInstance()),
    file_handler(),
    settings_handler(&file_handler),
    ui_handler(&file_handler) {
  file_handler.listDir("/", 0);

  process_data.ApplySettings(&settings_handler);
  process_data.Update(&rtc_handler, true);  // Check the date on first start

  // Set sleep timer if applicable
  uint8_t timeout = settings_handler.getScreenSetting(ui_handler.getCurrentScreen(), ScreenSetting::kSleepTimeout);
  if (timeout) {
    sleep_timeout = millis() + (timeout * 1000);
  }

  LOG_DEBUG_ARGS("battery voltage %f", device_handler.getBatteryVoltage());
}

EucSpeedo::~EucSpeedo() {
  delete wheel; // Clean up wheel pointer
  delete button_handler;  // Clean up button handler pointer
  if (config_server_active) {
    config_server->Stop();
    delete config_server;
  }
  if (ble_handler_active)
    delete ble_handler;
}

void EucSpeedo::Process() {
  HandlePress(button_handler->getPress());
  process_data.Update(&rtc_handler);
  process_data.Update(&device_handler);

  if (!config_server_active)  // Config server is asynchronous and takes over control of the UI
    ui_handler.Update(&process_data);

  if (ble_handler_active)
    ble_handler->Update();

  if (sleep_timeout && (sleep_timeout < millis()) && (!config_server_active) && (!ble_handler_active))
    HandleAction(Action::kOff);
}

// Creates the correct type of wheel object
void EucSpeedo::onFoundWheel(EucType type) {
  LOG_DEBUG_ARGS("Found %s EUC", kBrandName[(size_t)type]);
  ui_handler.ShowMessage(kBrandName[(size_t)type], 5);  // Show which brand is connected
  device_handler.LedOff();  // Stop LED flashing

  switch (type) {
    case EucType::kGotway:
      wheel = new Gotway();
      wheel_connected = true;
      break;
    case EucType::kKingsong:
      wheel = new Kingsong();
      wheel_connected = true;
      break;
    default:
      LOG_DEBUG("Wheel not yet implemented, uh oh!");
  }
}

void EucSpeedo::onProcessInput(uint8_t* data, size_t data_size) {
  if (wheel_connected) {
    wheel->ProcessInput(data, data_size);  // Let the specific wheel process the data
    process_data.Update(wheel); // Update the wheel data supplied to the UI
    // LOG_DEBUG_ARGS("Updated values\n\tspeed: %f\n\tvoltage: %f\n\tcurrent: %f\n\ttemperature: %f\n\tbattery_percent: %f\n\tdistance: %f\n\ttotal_distance: %f",
    //   wheel->getSpeed(), wheel->getVoltage(), wheel->getCurrent(), wheel->getTemperature(), wheel->getBatteryPercent(), wheel->getDistance(), wheel->getTotalDistance());
  }
}

void EucSpeedo::HandlePress(PressType press) {
  switch (press) {
    case PressType::kSinglePress:
      LOG_DEBUG("Single press");
      HandleAction(static_cast<Action>(settings_handler.getScreenSetting(ui_handler.getCurrentScreen(), ScreenSetting::kOnSinglePress)));
      break;
    case PressType::kDoublePress:
      LOG_DEBUG("Double press");
      HandleAction(static_cast<Action>(settings_handler.getScreenSetting(ui_handler.getCurrentScreen(), ScreenSetting::kOnDoublePress)));
      break;
    case PressType::kLongPress:
      LOG_DEBUG("Long press");
      HandleAction(static_cast<Action>(settings_handler.getScreenSetting(ui_handler.getCurrentScreen(), ScreenSetting::kOnLongPress)));
    default:
      break;
  }
}

void EucSpeedo::HandleAction(Action action) {
  LOG_DEBUG_ARGS("Action: %s", kActionNames[static_cast<uint8_t>(action)]);
  
  if (screen_sleep && action != Action::kOff) { // Any action turns the screen back on when screen sleeping
    ui_handler.Wake();
    screen_sleep = false;
    return;
  }

  if (config_server_active && action != Action::kActivateConfig) // No actions to be done while config server active except to shut down the server
    return;

  switch (action) {
    case Action::kOff:
      ui_handler.Sleep();
      device_handler.Sleep();
      break;
    case Action::kScreenSleep:
      ui_handler.Sleep();
      screen_sleep = true;
    case Action::kNextScreen: {
      uint8_t new_screen = (ui_handler.getCurrentScreen() + 1) % settings_handler.getNumScreens();

      for (;(new_screen != ui_handler.getCurrentScreen())
          && (settings_handler.getScreenSetting(new_screen, ScreenSetting::kOnlyConnected) > ble_handler_active);
          new_screen = (new_screen + 1) % settings_handler.getNumScreens()) {}
      
      if (new_screen == ui_handler.getCurrentScreen())
        break;

      ui_handler.ChangeScreen(new_screen);
      break;
    }
    case Action::kPreviousScreen: {
      uint8_t new_screen = (ui_handler.getCurrentScreen()? ui_handler.getCurrentScreen() - 1 : settings_handler.getNumScreens() - 1);

      for (;(new_screen != ui_handler.getCurrentScreen())
          && (settings_handler.getScreenSetting(new_screen, ScreenSetting::kOnlyConnected) > ble_handler_active);
          new_screen = (new_screen? new_screen - 1 : settings_handler.getNumScreens() - 1)) {}
      
      if (new_screen == ui_handler.getCurrentScreen())
        break;
        
      ui_handler.ChangeScreen(new_screen);
      break;
    }
    case Action::kActivateConfig: {
      if (config_server_active) {
        config_server->Stop();
        delete config_server;
        config_server_active = false;
        device_handler.LedOff();
      } else if (!ble_handler_active) { // Only one of BLE or Wifi may be active at a time due to memory constraints
        config_server = new ConfigServer(&ui_handler, &file_handler, &rtc_handler, &settings_handler);
        config_server->Start();
        config_server_active = true;
        device_handler.FlashLed(4); // 4 Hz
      }
      break;
    }
    case Action::kActivateBle: {
      if (ble_handler_active) {
        delete ble_handler;
        ble_handler_active = false;
      } else if (!config_server_active) { // Only one of BLE or Wifi may be active at a time due to memory constraints
        ble_handler = new BleHandler(std::bind(&EucSpeedo::onFoundWheel, this, std::placeholders::_1),
          std::bind(&EucSpeedo::onProcessInput, this, std::placeholders::_1, std::placeholders::_2));
        ble_handler_active = true;
        ble_handler->Scan([this](){
            device_handler.LedOff(); // Turn the LED off when the scan is finished
            if (!(ble_handler->isConnected() || ble_handler->isConnecting()))
              HandleAction(Action::kActivateBle); // Turn off ble handler if nothing to do
          });
        device_handler.FlashLed(8); // 8 Hz
      }
      break;
    }
    default:
      break;
  }

  if (!config_server_active && !ble_handler_active) {
    uint8_t timeout = settings_handler.getScreenSetting(ui_handler.getCurrentScreen(), ScreenSetting::kSleepTimeout);
    if (timeout) {
      sleep_timeout = millis() + (timeout * 1000);
    } else {
      sleep_timeout = 0;  // Resets if the new screen has no timeout
    }
  }
}

}