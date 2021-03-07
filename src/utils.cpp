#include "utils.h"

namespace euc {

int16_t Utils::FromTwos(uint16_t in) {
  return static_cast<int16_t>(in);
}

int32_t Utils::FromTwos32(uint32_t in) {
  return static_cast<int32_t>(in);
}

}