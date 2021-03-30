#ifndef EUC_INMOTION_H_
#define EUC_INMOTION_H_

#include "euc.h"

#include <functional>

namespace euc {

class Inmotion : public Euc {
  public:
    void ProcessInput(uint8_t data[], size_t data_len) override;

  private:
    struct CanMessage {
      CanMessage();
      CanMessage(uint8_t data[], size_t data_len);

      enum class IDValue : uint32_t {
        kNoOp = 0,
        kGetFastInfo = 0x0F550113,
        kGetSlowInfo = 0x0F550114,
        kRideMode = 0x0F550115,
        kPinCode = 0x0F550307,
        kLight = 0x0F55010D,
        kLed = 0x0F550116,  
        kHandleButton = 0x0F55012E,  
        kMaxSpeed = 0x0F550115,
        kSpeakerVolume = 0x0F55060A,
        kAlert = 0x0F780101
      };

      enum class CanFormat : bool {
        kStandard = 0,
        kExtended = 1
      };

      enum class CanFrame : bool {
        kData = 0,
        kRemote = 1
      };

      IDValue id = IDValue::kNoOp;
      uint8_t can_data[8];
      uint8_t len, ch;
      CanFormat format;
      CanFrame type;
      std::vector<uint8_t> extra_data;
      bool verified = false;
    };

    void ParseFastInfo(CanMessage message);
    void ParseAlert(CanMessage message);
    void ParseSlowInfo(CanMessage message);

    CanMessage Verify(uint8_t data[], size_t data_len);

    // Message data persistent for long messages
    std::vector<uint8_t> message_buffer;

    // EUC Data specific to Inmotion
    double angle, roll, speed_factor, temperature2, distance_factor;
    bool long_distance = false;

    std::function<double(double)> battery_percentage_calc;
};

}

#endif