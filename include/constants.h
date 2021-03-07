#ifndef CONSTANTS_H_
#define CONSTANTS_H_
namespace euc {

const static char* KINGSONG_DESCRIPTER_UUID = "00002902-0000-1000-8000-00805f9b34fb";
const static char* KINGSONG_READ_CHARACTER_UUID = "0000ffe1-0000-1000-8000-00805f9b34fb";
const static char* KINGSONG_SERVICE_UUID = "0000ffe0-0000-1000-8000-00805f9b34fb";

const static char* GOTWAY_READ_CHARACTER_UUID = "0000ffe1-0000-1000-8000-00805f9b34fb";
const static char* GOTWAY_SERVICE_UUID = "0000ffe0-0000-1000-8000-00805f9b34fb";

const static char* INMOTION_DESCRIPTER_UUID = "00002902-0000-1000-8000-00805f9b34fb";
const static char* INMOTION_READ_CHARACTER_UUID = "0000ffe4-0000-1000-8000-00805f9b34fb";
const static char* INMOTION_SERVICE_UUID = "0000ffe0-0000-1000-8000-00805f9b34fb";
const static char* INMOTION_WRITE_CHARACTER_UUID = "0000ffe9-0000-1000-8000-00805f9b34fb";
const static char* INMOTION_WRITE_SERVICE_UUID = "0000ffe5-0000-1000-8000-00805f9b34fb";

const static char* NINEBOT_Z_SERVICE_UUID = "6e400001-b5a3-f393-e0a9-e50e24dcca9e";
const static char* NINEBOT_Z_WRITE_CHARACTER_UUID = "6e400002-b5a3-f393-e0a9-e50e24dcca9e";
const static char* NINEBOT_Z_READ_CHARACTER_UUID = "6e400003-b5a3-f393-e0a9-e50e24dcca9e";
const static char* NINEBOT_Z_DESCRIPTER_UUID = "00002902-0000-1000-8000-00805f9b34fb";

const static char* NINEBOT_SERVICE_UUID = "0000ffe0-0000-1000-8000-00805f9b34fb";
const static char* NINEBOT_WRITE_CHARACTER_UUID = "0000ffe1-0000-1000-8000-00805f9b34fb";
const static char* NINEBOT_READ_CHARACTER_UUID = "0000ffe1-0000-1000-8000-00805f9b34fb";
const static char* NINEBOT_DESCRIPTER_UUID = "00002902-0000-1000-8000-00805f9b34fb";

enum class EucType : size_t {
  kKingsong = 0,
  kGotway = 1,
  kInmotion = 2,
  kNinebotZ = 3,
  kNinebot = 4
};

const static size_t kNumEucTypes = 5;
const static char* kServiceUuids[] = {KINGSONG_SERVICE_UUID, GOTWAY_SERVICE_UUID, INMOTION_SERVICE_UUID, NINEBOT_Z_SERVICE_UUID, NINEBOT_SERVICE_UUID};
const static char* kReadUuids[] = {KINGSONG_READ_CHARACTER_UUID, GOTWAY_READ_CHARACTER_UUID, INMOTION_READ_CHARACTER_UUID, NINEBOT_Z_READ_CHARACTER_UUID, NINEBOT_READ_CHARACTER_UUID};
const static char* kBrandName[] = {"Kingsong", "Gotway", "Inmotion", "NinebotZ", "Ninebot"};

}

#endif