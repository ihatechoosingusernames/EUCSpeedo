#ifndef FILEHANDLER_H_
#define FILEHANDLER_H_

#include <Arduino.h>

namespace euc {

class FileHandler {
  public:
    FileHandler();
    ~FileHandler();

    // Reads <size> bytes to array at data. If available bytes < size, size is changed to the number that there are
    void ReadFile(String file_name, char data[], size_t* size);
    // Reads a full CSV file of bytes
    std::vector<uint8_t> ReadCsvBytes(String file_name);
    // Returns file size in bytes
    size_t FileSize(String file_name);

    // Overwrites file with message
    void WriteFile(String file_name, String message);

    void AppendFile(String file_name, String message);
    void RenameFile(String file_name, String new_name);
    void DeleteFile(String file_name);

    void listDir(String dirname, uint8_t levels);
};

}

#endif