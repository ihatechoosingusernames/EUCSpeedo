#ifndef GOTWAY_H_
#define GOTWAY_H_

#include <Arduino.h>

namespace euc {
class Gotway {
    public:
        void ProcessInput(uint8_t data[], size_t data_len);

        double getSpeed();
        bool isVeteran();

    private:
        int voltage, current, temperature, temperature_2, battery_percent, voltage_scaler;
        bool is_veteran;
        long distance, total_distance;
        double speed;
        
        const double ratio = 0.875; // Gotway Magic number
};

}

#endif