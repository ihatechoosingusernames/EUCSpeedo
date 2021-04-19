#include "eucs/gotway.h"

#include <math.h>

#include "utils.h"

namespace euc {

void Gotway::ProcessInput(uint8_t data[], size_t data_len) {
  if (data_len >= 20) {
    uint8_t a1 = data[0];
    uint8_t a2 = data[1];
    uint8_t a3 = data[2];
    uint8_t a4 = data[3];
    uint8_t a5 = data[4];
    uint8_t a6 = data[5];
    uint8_t a19 = data[18];

    if ((a1 == 0xDC) && (a2 == 0x5A) && (a3 == 0x5C) && (a4 == 0x20)) {  // Sherman
      is_veteran = true;

      voltage = Utils::FromTwos((data[4]) << 8 | (data[5])) / 100;

      speed =  Utils::FromTwos((data[6]) << 8 | data[7]) / 10.0;

      distance = Utils::FromTwos32(data[10] << 24 | data[11] << 16 | data[8] << 8 | data[9]) / 1000.0;

      total_distance = Utils::FromTwos32(data[14] << 24 | data[15] << 16 | data[12] << 8 | data[13]) / 1000.0;

      current = Utils::FromTwos(data[16] << 8 | data[17]) * 10;

      temperature = (data[18]) << 8 | (data[19]);
      temperature_2 = temperature;

      if (voltage > 100.20) {
        battery_percent = 100;
      } else if (voltage > 81.60) {
        battery_percent = (voltage - 80.70) / 0.195;
      } else if (voltage > 79.35) {
        battery_percent = (voltage - 79.35) / 0.4875;
      } else {
        battery_percent = 0;
      }
      
      return;
    } else { // Gotway
      if (a1 != 85 || a2 != 170 || a19 != 0) {
        if (a1 != 90 || a5 != 85 || a6 != 170) {
          return;
        }
        total_distance = ((data[6]) << 24) | ((data[7]) << 16) | ((data[8]) << 8) | (data[9]);

        return;
      }

      speed = abs(Utils::FromTwos((data[4] << 8) | data[5]) * 3.6) / 100.0;

      temperature = ((Utils::FromTwos(data[12] << 8) | data[13]) / 340.0) + 35;
      temperature_2 = temperature;

      distance = Utils::FromTwos((data[9] << 8) | data[8]);

      double temp_voltage = Utils::FromTwos((data[2] << 8) | data[3]) / 100;

      current = Utils::FromTwos((data[10] << 8) | data[11]) / 100;

      if (temp_voltage > 66.80) {
        battery_percent = 100;
      } else if (temp_voltage > 54.40) {
        battery_percent = (temp_voltage - 53.80) / 0.13;
      } else if (temp_voltage > 52.90) {
        battery_percent = (temp_voltage - 52.90) / 0.325;
      } else {
        battery_percent = 0;
      }

      voltage = temp_voltage * (1 + (0.25 * voltage_scaler));
    }

    return;

  } else if (data_len >= 10 && !is_veteran)  {
    int a1 = data[0];
    int a5 = data[4];
    int a6 = data[5];
    if (a1 != 90 || a5 != 85 || a6 != 170) {
      return;
    }
    total_distance = ((data[6]&0xFF) <<24) | ((data[7]&0xFF) << 16) | ((data[8]) <<8) | (data[9]);
  }
}

bool Gotway::isVeteran() {
  return is_veteran;
}

}
