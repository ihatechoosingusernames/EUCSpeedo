#ifndef CONSTANTS_H_
#define CONSTANTS_H_
namespace euc {

const static char* kKingsong_descripter_uuid = "00002902-0000-1000-8000-00805f9b34fb";
const static char* kKingsong_read_character_uuid = "0000ffe1-0000-1000-8000-00805f9b34fb";
const static char* kKingsong_service_uuid = "0000ffe0-0000-1000-8000-00805f9b34fb";

const static char* kGotway_read_character_uuid = "0000ffe1-0000-1000-8000-00805f9b34fb";
const static char* kGotway_service_uuid = "0000ffe0-0000-1000-8000-00805f9b34fb";

const static char* kInmotion_descripter_uuid = "00002902-0000-1000-8000-00805f9b34fb";
const static char* kInmotion_read_character_uuid = "0000ffe4-0000-1000-8000-00805f9b34fb";
const static char* kInmotion_service_uuid = "0000ffe0-0000-1000-8000-00805f9b34fb";
const static char* kInmotion_write_character_uuid = "0000ffe9-0000-1000-8000-00805f9b34fb";
const static char* kInmotion_write_service_uuid = "0000ffe5-0000-1000-8000-00805f9b34fb";

const static char* kNinebot_z_service_uuid = "6e400001-b5a3-f393-e0a9-e50e24dcca9e";
const static char* kNinebot_z_write_character_uuid = "6e400002-b5a3-f393-e0a9-e50e24dcca9e";
const static char* kNinebot_z_read_character_uuid = "6e400003-b5a3-f393-e0a9-e50e24dcca9e";
const static char* kNinebot_z_descripter_uuid = "00002902-0000-1000-8000-00805f9b34fb";

const static char* kNinebot_service_uuid = "0000ffe0-0000-1000-8000-00805f9b34fb";
const static char* kNinebot_write_character_uuid = "0000ffe1-0000-1000-8000-00805f9b34fb";
const static char* kNinebot_read_character_uuid = "0000ffe1-0000-1000-8000-00805f9b34fb";
const static char* kNinebot_descripter_uuid = "00002902-0000-1000-8000-00805f9b34fb";

enum class EucType : size_t {
  kKingsong = 0,
  kGotway = 1,
  kInmotion = 2,
  kNinebotZ = 3,
  kNinebot = 4
};

const static size_t kNumEucTypes = 5;
const static char* kServiceUuids[] = {kKingsong_service_uuid, kGotway_service_uuid, kInmotion_service_uuid, kNinebot_z_service_uuid, kNinebot_service_uuid};
const static char* kReadUuids[] = {kKingsong_read_character_uuid, kGotway_read_character_uuid, kInmotion_read_character_uuid, kNinebot_z_read_character_uuid, kNinebot_read_character_uuid};
const static char* kBrandName[] = {"Kingsong", "Gotway", "Inmotion", "NinebotZ", "Ninebot"};

enum class PressType : size_t {
  kNoPress = 0,
  kSinglePress,
  kDoublePress,
  kLongPress
};

const static long kMinPressTime = 150;  // Minimum button press time in milliseconds.
const static long kMaxPressTime = 3000; // Maximum button press time before it counts as a long press
const static long kMaxReleaseTime = 150;  // Maximum button release time between double presses in milliseconds.

}

#endif