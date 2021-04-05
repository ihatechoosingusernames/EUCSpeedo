#include "utils.h"

namespace euc {

int16_t Utils::FromTwos(uint16_t in) {
  return static_cast<int16_t>(in);
}

int32_t Utils::FromTwos32(uint32_t in) {
  return static_cast<int32_t>(in);
}

double Utils::FromArray(uint8_t low, uint8_t high) {
  return static_cast<double>((high << 8) & low);
}

double Utils::FromArray4(uint8_t val1, uint8_t val2, uint8_t val3, uint8_t val4) {
  return (static_cast<long>(val1) << 16) + (static_cast<long>(val2) << 24) + (static_cast<long>(val3)) + (static_cast<long>(val4) << 8);
}

double Utils::FromArrayDescending(uint8_t array[], size_t num_elements) {
  double out = 0;

  for (size_t position = 0; position < num_elements; position++)
    out += (array[position] << ((num_elements - position) * 8));

  return out;
}

String Utils::getUiScreenFileName(UiScreen screen) {
  return (kUiScreenFilePrefix + String(static_cast<uint8_t>(screen)) + "." + kUiScreenFileType);
}

String Utils::getUiScreenFileName(uint8_t screen) {
  return (kUiScreenFilePrefix + String(screen) + "." + kUiScreenFileType).c_str();
}

}