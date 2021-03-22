#include "eucs/kingsong.h"

#include "utils.h"

namespace euc {

void Kingsong::ProcessInput(uint8_t data[], size_t data_len) {
  if (data_len >= 20) {
    int a1 = data[0] & 255;
    int a2 = data[1] & 255;

    if (a1 != 170 || a2 != 85) {
        return;
    }

    if (data[16] == 169) { // Live data
      voltage = Utils::FromArray(data[2], data[3]);
      
      speed = Utils::FromArray(data[4], data[5]);

      total_distance = Utils::FromArray4(data[6], data[7], data[8], data[9]);

      if (model == "KS-18L") {
        total_distance = total_distance * ks18l_scaler;
      }

      current = ((data[10]&0xFF) + (data[11]<<8));

      temperature = Utils::FromArray(data[12], data[13]);
        // setVoltageSag(voltage);

      if (data[15] == 224) {
        mode = data[14];
        // mModeStr = String.format(Locale.US, "%d", mMode);
      }

      if ((model.compareTo("KS-18L") == 0) || (model.compareTo("KS-16X") == 0) || (model.compareTo("KS-18LH") == 0) || (model.compareTo("KS-S18") == 0)) {
        if (voltage < 6250) {
            battery_percent = 0;
        } else if (voltage >= 8250) {
            battery_percent = 100;
        } else {
            battery_percent = (voltage - 6250) / 20;
        }
      } else {
        if (voltage < 5000) {
            battery_percent = 0;
        } else if (voltage >= 6600) {
            battery_percent = 100;
        } else {
            battery_percent = (voltage - 5000) / 16;
        }
      }
      return;

    } else if (data[16] == 185) { // Distance/Time/Fan Data
        distance = Utils::FromArray4(data[2], data[3], data[4], data[5]);

        // setTopSpeed(byteArrayInt2(data[8], data[9]));
        // mFanStatus = data[12];
        return;
    } else if (data[16] == 187) { // Name and Type data
        String model_string = "";
        for (size_t index = 2; index < 16 && data[index] != 0; index++) // Parse model name
          model_string += static_cast<char>(data[index]);

        model_string.trim();

        model = model_string.substring(0, model_string.lastIndexOf('-')); // Cut off last '-' delimited substring

        return;

    } else if (data[16] == 179) { // Serial Number, ignoring this for now
        // byte[] sndata = new byte[18];
        // System.arraycopy(data, 2, sndata, 0, 14);
        // System.arraycopy(data, 17, sndata, 14, 3);
        // sndata[17] = (byte) 0;
        // mSerialNumber = new String(sndata);
        // updateKSAlarmAndSpeed();
        return;
    }
    else if (data[16] == 164 || data[16] == 181) { //0xa4 || 0xb5 max speed and alerts
        max_speed = data[10];
        alarm_3_speed = data[8];
        alarm_2_speed = data[6];
        alarm_1_speed = data[4];

        // after received 0xa4 send same repeat data[2] =0x01 data[16] = 0x98
        if(data[16] == 164)
        {
            data[16] = 0x98;
            // mBluetoothLeService.writeBluetoothGattCharacteristic(data);  // Need to figure out how and why
        }
        return;
    }
  }
  return;
}

}