#include "eucs/inmotion.h"

#include "utils.h"

namespace euc {

void Inmotion::ProcessInput(uint8_t data[], size_t data_len) {
  bool last_char = false;
  for (size_t byte = 0; byte < data_len; byte++) {
    message_buffer.emplace_back(data[byte]);

    if (data[byte] == 0x55) {
      if (!last_char) {
        last_char = true;
      } else {
        CanMessage message = Verify(message_buffer.data(), message_buffer.size());

        if (message.verified) {
          switch (message.id) {
            case CanMessage::IDValue::kGetFastInfo:
              ParseFastInfo(message);
              return ;
            case CanMessage::IDValue::kAlert:
              ParseAlert(message);
              return;
            case CanMessage::IDValue::kGetSlowInfo:
              ParseSlowInfo(message);
          }
        }

        message_buffer.clear();
        message_buffer.emplace_back(0xAA);
        message_buffer.emplace_back(0xAA);
      }
    }
  }
}

Inmotion::CanMessage::CanMessage() {};

Inmotion::CanMessage::CanMessage(uint8_t data[], size_t data_len) {
  if (data_len < 16) {
    printf("Inmotion::CanMessage() data len too short\n");
    return;
  }
  
  id = static_cast<IDValue>(Utils::FromArrayLsb<uint32_t>(data, 4));
  std::copy(data + 4, data + 12, can_data);
  len = data[12];
  ch = data[13];
  format = static_cast<CanFormat>(data[14]);
  type = static_cast<CanFrame>(data[15]);

  if (len == 0xFE) {
    int32_t ldata = Utils::FromArrayLsb<int32_t>(data, 4);

    if (ldata == data_len - 16) {
      extra_data = std::vector<uint8_t>(data + 16, data + data_len);
    }
  }

  verified = true;
}

  // if (unpacker.addChar(c)) {
  //   CanMessage result = CanMessage.verify(unpacker.getBuffer());
  //   if (result != null) { // data OK
  //     if (result.id == CanMessage.IDValue.GetFastInfo.getValue()) {
  //       Status vals = result.parseFastInfoMessage(model);
  //       if (vals != null)
  //         outValues.add(vals);
 
  //     } else if (result.id == CanMessage.IDValue.Alert.getValue()){
  //       Alert alert = result.parseAlertInfoMessage();
  //       if (alert != null)
  //         outValues.add(alert);
        
  //     } else if (result.id == CanMessage.IDValue.GetSlowInfo.getValue()){
  //       Infos infos = result.parseSlowInfoMessage();
  //       if (infos != null) {
  //         needSlowData = false;
  //         model = infos.getModel();
  //         outValues.add(infos);
  //       }
  //     }
  //   }
  // }

void Inmotion::ParseFastInfo(CanMessage message) {
  if (message.extra_data.size() < 76) {
    printf("Inmotion::ParseFastInfo() failed due to not enough extra_data\n");
    return;
  }

  angle = Utils::FromArrayLsb<int32_t>(message.extra_data.data(), 4) / 65536.0;
  roll = Utils::FromArrayLsb<int32_t>(message.extra_data.data() + 72, 4) / 90;  // (double) (this.intFromBytes(ex_data, 72)) / 90.0;
  speed = std::abs(Utils::FromArrayLsb<int32_t>(message.extra_data.data() + 12, 4) + Utils::FromArrayLsb<int32_t>(message.extra_data.data() + 16, 4)) / (speed_factor * 2); // ((double) (this.signedIntFromBytes(ex_data, 12)) + (double) (this.signedIntFromBytes(ex_data, 16))) / (model.getSpeedCalculationFactor() * 2.0);

  voltage = Utils::FromArrayLsb<int32_t>(message.extra_data.data() + 24, 4) / 100; // (double) (this.intFromBytes(ex_data, 24)) / 100.0;
  current = Utils::FromArrayLsb<int32_t>(message.extra_data.data() + 20, 4) / 100; // (double) (this.signedIntFromBytes(ex_data, 20)) / 100.0;
  temperature = message.extra_data[32]; // ex_data[32] & 0xff;
  temperature2 = message.extra_data[34]; // ex_data[34] & 0xff;
  battery_percent = battery_percentage_calc(voltage);

  if (long_distance)
    distance = Utils::FromArrayLsb<int32_t>(message.extra_data.data() + 44, 4) / distance_factor;
  else
    distance = Utils::FromArrayLsb<int64_t>(message.extra_data.data() + 44, 8) / distance_factor;

  // if (model.belongToInputType( "1") || model.belongToInputType( "5") ||
  //         model == V8 || model == Glide3 || model == V10 || model == V10F || model == V10_test || model == V10F_test) {
  //     distance = (double) (this.intFromBytes(ex_data, 44)) / 1000.0d; ///// V10F 48 byte - trip distance
  // } else if (model == R0) {
  //     distance = (double) (this.longFromBytes(ex_data, 44)) / 1000.0d;

  // } else if (model == L6) {
  //     distance = (double) (this.longFromBytes(ex_data, 44)) / 10.0;

  // } else {
  //     distance = (double) (this.longFromBytes(ex_data, 44)) / 5.711016379455429E7d;
  // }

  // int workModeInt = this.intFromBytes(ex_data, 60)&0xF;
  // WorkMode workMode = intToWorkMode(workModeInt);
  // double lock = 0.0;
  // switch (workMode) {

  //     case lock:
  //         lock = 1.0;

  //     default:
  //         break;
  // }
}

void Inmotion::ParseAlert(CanMessage message) {
  
}

void Inmotion::ParseSlowInfo(CanMessage message) {
  
}

Inmotion::CanMessage Inmotion::Verify(uint8_t data[], size_t data_len) {
  if (data[0] != 0xAA || data[1] != 0xAA || data[data_len - 1] != 0x55 || data[data_len - 2] != 0x55) {
    printf("Inmotion::Verify() Header or tail incorrect\n");
    return CanMessage();  // Header and tail not correct
  }

  std::vector<uint8_t> data_buffer = std::vector<uint8_t>(data + 2, data + (data_len - 3));

  for (std::vector<uint8_t>::iterator byte = data_buffer.begin(); byte != data_buffer.end(); byte++) {
    if (*byte == 0xA5)
      data_buffer.erase(byte, byte + 2);  // Erase this and the next byte
  }

  uint8_t check;
  for (uint8_t byte : data_buffer)
    check += byte;

  uint8_t buffer_check = data[data_len - 3];

  if (check == buffer_check) {
      return CanMessage(data_buffer.data(), data_buffer.size());
  }

  printf("Inmotion::Verify failed, calc: %X, packet: %X", check, buffer_check);
  return CanMessage();
}

}