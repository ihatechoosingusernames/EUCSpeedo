#ifndef FILEHANDLER_H_
#define FILEHANDLER_H_

#include <Arduino.h>
#include <list>

namespace euc {

class FileHandler {
  public:
    FileHandler();
    ~FileHandler();

    // Reads <size> bytes to array at data. If available bytes < size, size is changed to the number that there are
    void ReadFile(const char* file_name, char data[], size_t* size);
    // Reads a full CSV file of bytes
    std::list<uint8_t> ReadCsvBytes(const char* file_name);
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