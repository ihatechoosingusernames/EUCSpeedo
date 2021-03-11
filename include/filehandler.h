#ifndef FILEHANDLER_H_
#define FILEHANDLER_H_

#include <Arduino.h>

namespace euc {

class FileHandler {
  public:
    FileHandler();

    uint8_t* ReadFile(const char* file_name, size_t* size);
};

}

#endif