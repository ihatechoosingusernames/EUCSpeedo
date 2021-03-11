#include "filehandler.h"

#include "SPIFFS.h"

namespace euc {

FileHandler::FileHandler() {
  if (!SPIFFS.begin(true)) {
    Serial.println("An Error has occurred while mounting SPIFFS");
  }
}

void FileHandler::ReadFile(const char * path, uint8_t data[], size_t* size) {
  File file = SPIFFS.open(path);

  if(!file || file.isDirectory()){
    Serial.println("- failed to open file for reading");
    *size = 0;
    return;
  }

  if (file.size() < *size)
    *size = file.size();

  file.read(data, *size);
  
  file.close();
}

size_t FileHandler::FileSize(const char * filename) {
  File file = SPIFFS.open(filename);

  if(!file || file.isDirectory()){
    Serial.println("- failed to open file for checking size");
    return 0;
  }

  return file.size();
}

void FileHandler::WriteFile(const char * file_name, const char * message) {
    File file = SPIFFS.open(file_name, FILE_WRITE);
    if(!file){
      Serial.println("- failed to open file for writing");
      return;
    }
    if(!file.print(message)){
      Serial.println("- write failed");
    }
    file.close();
}

void FileHandler::AppendFile(const char * file_name, const char * message) {
  File file = SPIFFS.open(file_name, FILE_APPEND);
  if(!file){
    Serial.println("- failed to open file for appending");
    return;
  }
  if(!file.print(message)){
    Serial.println("- append failed");
  }
  file.close();
}

void FileHandler::RenameFile(const char * file_name1, const char * new_name) {
  if (!SPIFFS.rename(file_name1, new_name)) {
    Serial.println("- rename failed");
  }
}

void FileHandler::DeleteFile(const char * file_name) {
  if(!SPIFFS.remove(file_name)){
    Serial.println("- delete failed");
  }
}

}