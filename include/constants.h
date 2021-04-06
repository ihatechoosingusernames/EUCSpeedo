#ifndef CONSTANTS_H_
#define CONSTANTS_H_

#include <array>
#include <vector>
namespace euc {

enum class EucType : size_t { // The order here is important, as it is from least to most unique service list. And yes I agree, that is hacky.
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
const static std::vector<uint8_t> kManufacturer[] = {{0x4, 0x8, 0x4, 0xd, 0x8, 0x8, 0x2, 0x5, 0x8, 0x3, 0xf, 0x0, 0xf, 0xf, 0xc, 0x4}, {}, {}, {}, {}};

// const static std::vector<const char*> kGotwayServices = {"00001800_0000_1000_8000_00805f9b34fb", "00001801_0000_1000_8000_00805f9b34fb", "0000180a_0000_1000_8000_00805f9b34fb", "0000ffe0_0000_1000_8000_00805f9b34fb"};
// const static std::vector<const char*> kKingSongServices = {"00001800_0000_1000_8000_00805f9b34fb", "00001801_0000_1000_8000_00805f9b34fb", "0000180a_0000_1000_8000_00805f9b34fb", "0000fff0_0000_1000_8000_00805f9b34fb", "0000ffe0_0000_1000_8000_00805f9b34fb"};
// const static std::vector<const char*> kInmotionServices = {"0000180a_0000_1000_8000_00805f9b34fb", "0000180f_0000_1000_8000_00805f9b34fb", "0000ffe0_0000_1000_8000_00805f9b34fb", "0000ffe5_0000_1000_8000_00805f9b34fb", "0000fff0_0000_1000_8000_00805f9b34fb", "0000ffd0_0000_1000_8000_00805f9b34fb", "0000ffc0_0000_1000_8000_00805f9b34fb", "0000ffb0_0000_1000_8000_00805f9b34fb", "0000ffa0_0000_1000_8000_00805f9b34fb", "0000ff90_0000_1000_8000_00805f9b34fb", "0000fc60_0000_1000_8000_00805f9b34fb", "0000fe00_0000_1000_8000_00805f9b34fb"};
// const static std::vector<const char*> kNinebotZServices = {"00001800_0000_1000_8000_00805f9b34fb", "00001801_0000_1000_8000_00805f9b34fb", "6e400001_b5a3_f393_e0a9_e50e24dcca9e", "0000fee7_0000_1000_8000_00805f9b34fb"};
// const static std::vector<const char*> kNinebotServices = {"00001800_0000_1000_8000_00805f9b34fb", "00001801_0000_1000_8000_00805f9b34fb", "0000ffe0_0000_1000_8000_00805f9b34fb"};

// const static std::array<std::vector<const char*>, 5> kServiceList = {kGotwayServices, kKingSongServices, kInmotionServices, kNinebotZServices, kNinebotServices};

enum class PressType : size_t {
  kNoPress = 0,
  kSinglePress,
  kDoublePress,
  kLongPress
};

const static long kMinPressTime = 150;  // Minimum button press time in milliseconds.
const static long kMaxPressTime = 3000; // Maximum button press time before it counts as a long press
const static long kMaxReleaseTime = 300;  // Maximum button release time between double presses in milliseconds.

const static char* kUiScreenFilePrefix = "/ui_config";
const static char* kUiScreenFileType = "csv";

enum class UiScreen : uint8_t { // These map to the config screen files: ui_config0.csv, ui_config1.csv etc.
  kHome = 0,
  kConfig,
  kCustom
};

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
  kLastValue  // IMPORTANT this value should stay as last value
};

const static char* kDataTypeNames[] = {"Speed", "Second", "Minute", "Hour", "Day", "Month", "Year", "Watch Battery", "Battery", "Temperature", "Alarm Level", "Trip Distance", "Total Distance", "Voltage", "Current"};

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
const static char* kDefaultURL = "eucspeedo";

}

#endif