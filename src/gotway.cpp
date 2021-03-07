#include <math.h>

#include "gotway.h"

void Gotway::ProcessInput(uint8_t data[], size_t data_len) {
    if (data_len >= 20) {
        int a1 = data[0] & 255;
        int a2 = data[1] & 255;
        int a3 = data[2] & 255;
        int a4 = data[3] & 255;
        int a5 = data[4] & 255;
        int a6 = data[5] & 255;
        int a19 = data[18] & 255;

        if ((a1 == 0xDC) && (a2 == 0x5A) && (a3 == 0x5C) && (a4 == 0x20)) {  // Sherman
            is_veteran = true;

            voltage = (data[4] & 0xFF) << 8 | (data[5] & 0xFF);

            speed =  ((data[6]) << 8 | (data[7] & 0xFF))*10;

            distance = ((data[10] & 0xFF) << 24 | (data[11] & 0xFF) << 16 | (data[8] & 0xFF) << 8 | (data[9] & 0xFF));

            total_distance = ((data[14] & 0xFF) << 24 | (data[15] & 0xFF) << 16 | (data[12] & 0xFF) << 8 | (data[13] & 0xFF));

            current = ((data[16]) << 8 | (data[17] & 0xFF))*10;

            temperature = (data[18] & 0xFF) << 8 | (data[19] & 0xFF);
            temperature_2 = temperature;

            int battery;

            if (voltage <= 7935) {
                battery = 0;
            } else if (voltage >= 9870) {
                battery = 100;
            } else {
                battery = (int) nearbyint((voltage - 7935) / 19.5);
            }

            battery_percent = battery;
            
            return;
        } else { // Gotway
            if (a1 != 85 || a2 != 170 || a19 != 0) {
                if (a1 != 90 || a5 != 85 || a6 != 170) {
                    return;
                }
                total_distance = ((data[6] & 0xFF) << 24) | ((data[7] & 0xFF) << 16) | ((data[8] & 0xFF) << 8) | (data[9] & 0xFF);
            }

            if (data[5] >= 0) {
                speed = abs(((data[4] << 8) & data[5]) * 3.6);
            } else {
                speed = abs((((data[4] << 8) + 256.0) + data[5]) * 3.6);
            }

            temperature = nearbyint(((((data[12] << 8) + data[13]) / 340.0) + 35) * 100);
            temperature_2 = temperature;

            distance = ByteArrayInt2(data[9], data[8]);

            voltage = (data[2] << 8) + (data[3] & 255);

            current = ((data[10] << 8) + data[11]);

            int battery;

            if (voltage <= 5290) {
                battery = 0;
            } else if (voltage >= 6580) {
                battery = 100;
            } else {
                battery = (voltage - 5290) / 13;
            }

            battery_percent = battery;

            voltage = nearbyint(voltage * (1 + (0.25 * voltage_scaler)));
        }

        return;

    } else if (data_len >= 10 && !is_veteran)  {
        int a1 = data[0];
        int a5 = data[4] & 255;
        int a6 = data[5] & 255;
        if (a1 != 90 || a5 != 85 || a6 != 170) {
            return;
        }
        total_distance = ((data[6]&0xFF) <<24) | ((data[7]&0xFF) << 16) | ((data[8] & 0xFF) <<8) | (data[9] & 0xFF);
    }
}

double Gotway::getSpeed() {
    return speed / 100.0;
}

bool Gotway::isVeteran() {
    return is_veteran;
}

int Gotway::ByteArrayInt2(uint8_t low, uint8_t high) {
    return (low & 255) + ((high & 255) << 8);
}
