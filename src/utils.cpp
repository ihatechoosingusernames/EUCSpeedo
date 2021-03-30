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

const char* Utils::getUiScreenFileName(UiScreen screen) {
  return (kUiScreenFilePrefix + String(static_cast<uint8_t>(screen)) + "." + kUiScreenFileType).c_str();
}

const char* Utils::getUiScreenFileName(uint8_t screen) {
  return (kUiScreenFilePrefix + String(screen) + "." + kUiScreenFileType).c_str();
}

}