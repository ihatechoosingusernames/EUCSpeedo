#ifndef FILEHANDLER_H_
#define FILEHANDLER_H_

#include <Arduino.h>

namespace euc {

class FileHandler {
  public:
    FileHandler();
    ~FileHandler();

    // Reads <size> bytes to array at data. If available bytes < size, size is changed to the number that there are
    void ReadFile(const char* file_name, uint8_t data[], size_t* size);
    // Returns file size in bytes
    size_t FileSize(const char* file_name);

    // Overwrites file with message
    void WriteFile(const char * file_name, const char * message);

    void AppendFile(const char * file_name, const char * message);
    void RenameFile(const char * file_name, const char * new_name);
    void DeleteFile(const char * file_name);

    void listDir(const char * dirname, uint8_t levels);
};

}

#endif