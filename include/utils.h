#ifndef UTILS_H_
#define UTILS_H_

#include <Arduino.h>

namespace euc {

class Utils {
    public:
        static int16_t FromTwos(uint16_t in);
};

}

#endif