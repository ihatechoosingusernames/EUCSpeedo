#ifndef CONSTANTS_H_
#define CONSTANTS_H_

#include <array>
#include <vector>
namespace euc {

enum class EucType : size_t {
  kGotway = 0,
  kKingsong,
  kInmotion,
  kNinebotZ,
  kNinebot,
  kLastType // IMPORTANT, this marks the last type of EUC, and the size of the EUC arrays
};

const static char* kServiceUuids[] = {"0000ffe0-0000-1000-8000-00805f9b34fb", "0000ffe0-0000-1000-8000-00805f9b34fb", "0000ffe0-0000-1000-8000-00805f9b34fb", "6e400001-b5a3-f393-e0a9-e50e24dcca9e", "0000ffe0-0000-1000-8000-00805f9b34fb"};
const static char* kReadCharacteristicUuids[] = {"0000ffe1-0000-1000-8000-00805f9b34fb", "0000ffe1-0000-1000-8000-00805f9b34fb", "0000ffe4-0000-1000-8000-00805f9b34fb", "6e400003-b5a3-f393-e0a9-e50e24dcca9e", "0000ffe1-0000-1000-8000-00805f9b34fb"};
const static char* kBrandName[] = {"GOTWAY", "KINGSONG", "INMOTION", "NINEBOTZ", "NINEBOT"};

// Using vectors for these so they can be referenced by kServiceList, and provide their own size. std::arrays would all have to be the same length.
const static std::vector<const char*> kGotwayServices = {"00001800-0000-1000-8000-00805f9b34fb", "00001801-0000-1000-8000-00805f9b34fb", "0000180a-0000-1000-8000-00805f9b34fb", "0000ffe0-0000-1000-8000-00805f9b34fb"};
const static std::vector<const char*> kKingSongServices = {"00001800-0000-1000-8000-00805f9b34fb", "00001801-0000-1000-8000-00805f9b34fb", "0000180a-0000-1000-8000-00805f9b34fb", "0000fff0-0000-1000-8000-00805f9b34fb", "0000ffe0-0000-1000-8000-00805f9b34fb"};
const static std::vector<const char*> kInmotionServices = {"0000180a-0000-1000-8000-00805f9b34fb", "0000180f-0000-1000-8000-00805f9b34fb", "0000ffe0-0000-1000-8000-00805f9b34fb", "0000ffe5-0000-1000-8000-00805f9b34fb", "0000fff0-0000-1000-8000-00805f9b34fb", "0000ffd0-0000-1000-8000-00805f9b34fb", "0000ffc0-0000-1000-8000-00805f9b34fb", "0000ffb0-0000-1000-8000-00805f9b34fb", "0000ffa0-0000-1000-8000-00805f9b34fb", "0000ff90-0000-1000-8000-00805f9b34fb", "0000fc60-0000-1000-8000-00805f9b34fb", "0000fe00-0000-1000-8000-00805f9b34fb"};
const static std::vector<const char*> kNinebotZServices = {"00001800-0000-1000-8000-00805f9b34fb", "00001801-0000-1000-8000-00805f9b34fb", "6e400001-b5a3-f393-e0a9-e50e24dcca9e", "0000fee7-0000-1000-8000-00805f9b34fb"};
const static std::vector<const char*> kNinebotServices = {"00001800-0000-1000-8000-00805f9b34fb", "00001801-0000-1000-8000-00805f9b34fb", "0000ffe0-0000-1000-8000-00805f9b34fb"};

const static std::array<std::vector<const char*>, 5> kServiceList = {kGotwayServices, kKingSongServices, kInmotionServices, kNinebotZServices, kNinebotServices};

enum class PressType : size_t {
  kNoPress = 0,
  kSinglePress,
  kDoublePress,
  kLongPress
};

const static long kMinPressTime = 150;  // Minimum button press time in milliseconds.
const static long kMaxPressTime = 3000; // Maximum button press time before it counts as a long press
const static long kMaxReleaseTime = 300;  // Maximum button release time between presses in a double press in milliseconds.

const static char* kUiScreenFilePrefix = "/ui_config";
const static char* kUiScreenFileType = "csv";

const static uint8_t kBigNumberFont = 7;
const static char* kSmallTextFont = "FiraCodeBold20";

const static size_t kMaxUiElementCode = 255;

enum class DataType : uint8_t {
  kSpeed = 0,
  kSecond,
  kMinute,
  kHour,
  kDay,
  kMonth,
  kYear,
  kWatchBattery,
  kBattery,
  kTemp,
  kAlarmLevel,
  kTripDistance,
  kTotalDistance,
  kVoltage,
  kCurrent,
  kPower,
  kLastValue  // IMPORTANT this value should stay as last value
};

const static char* kDataTypeNames[] = {"Speed", "Second", "Minute", "Hour", "Day", "Month", "Year", "Watch Battery (percent)", "Battery (percent)", "Temperature", "Alarm Level", "Trip Distance", "Total Distance", "Voltage", "Current", "Power (W/100)"};

enum class ColourType : uint8_t {
  kConstant = 0,
  kDynamicBetweenValues
};

enum class ArgType : uint8_t {
  kDataType = 0,
  kColour,
  kConstant
};

const static char* kArgTypeNames[] = {"Data Type", "Colour", "Constant"};

// These define the default UI screen, currently a blank green screen
const static uint8_t kUiDefaultPreferences[] = {0x01, static_cast<uint8_t>(ColourType::kConstant), 0x00, 0xFF, 0x00};
const static size_t kUiDefaultPreferencesLength = 5;

// Default port for config server
const static int kDefaultServerPort = 80;
const static char* kDefaultServerSSID = "EUC-Speedo";
const static char* kDefaultURL = "speedo";

enum class GeneralSetting : uint8_t {
  kTemperatureUnits = 0,
  kDistanceUnits,
  kOrientation,
  kLastValue  // IMPORTANT this value should stay as last value
};

const static char* kGeneralSettingsFile = "/general_settings.csv";

enum class ScreenSetting : uint8_t {
  kOnlyConnected = 0,
  kOnSinglePress,
  kOnDoublePress,
  kOnLongPress,
  kSleepTimeout,
  kOffTimeout,
  kLastValue  // IMPORTANT this value should stay as last value
};

enum class Action : uint8_t {
  kOff = 0,
  kScreenSleep,
  kNextScreen,
  kPreviousScreen,
  kActivateConfig,
  kActivateBle,
  kLastValue  // IMPORTANT this value should stay as last value
};

const static char* kActionNames[] = {"Off","Sleep", "Next Screen", "Previous Screen", "Activate Config Server", "Activate Bluetooth"};

const static double kMphConversionFactor = 0.6213712;
const static double kFreedomsConversionFactor = 1.8;
const static double kFreedomsConversionOffset = 32;

const static double kVoltageRef = 1100;

const static char* kFirmwareFilename = "firmware.bin";
const static char* kSpiffsFilename = "spiffs.bin";

// MPU9250 constants
const static uint8_t kMPU9250_ADDRESS = 0x69;
const static uint8_t kPWR_MGMT_1 = 0x6B;
const static uint8_t kPWR1_SLEEP_BIT = 0x6;

}

#endif