#ifndef UTILS_H_
#define UTILS_H_

#include <Arduino.h>

namespace euc {

class Utils {
    public:
        static int16_t FromTwos(uint16_t in);
        static int32_t FromTwos32(uint32_t in);
};

}

#endif