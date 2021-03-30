#ifndef UTILS_H_
#define UTILS_H_

#include <Arduino.h>

#include "constants.h"
namespace euc {

class Utils {
  public:
    // Converts from two's complement raw bytes to a signed int
    static int16_t FromTwos(uint16_t in);
    static int32_t FromTwos32(uint32_t in);

    // Interprets two bytes into a double
    static double FromArray(uint8_t low, uint8_t high);

    static double FromArray4(uint8_t val1, uint8_t val2, uint8_t val3, uint8_t val4);

    // Interprets num_elements of the array in descending order of byte position (MSB, LSB, ...)
    template <typename T>
    static T FromArrayLsb(uint8_t array[], size_t num_elements) {
      T out = 0;

      for (size_t position = 0; position < num_elements; position++)
        out |= (array[position] << ((num_elements - position) * 8));

      return out;
    }

    static const char* getUiScreenFileName(UiScreen screen);
    static const char* getUiScreenFileName(uint8_t screen);
};

}

#endif