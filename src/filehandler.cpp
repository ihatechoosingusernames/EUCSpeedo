#include "filehandler.h"

#include "SPIFFS.h"

namespace euc {

FileHandler::FileHandler() {
  if (!SPIFFS.begin()) {
    Serial.println("An Error has occurred while mounting SPIFFS");
  }
}

FileHandler::~FileHandler() {
  SPIFFS.end();
}

void FileHandler::ReadFile(const char * path, char data[], size_t* size) {
  File file = SPIFFS.open(path, FILE_READ);

  if(!file || file.isDirectory()){
    Serial.println("failed to open file for reading");
    *size = 0;
    return;
  }

  if (file.size() < *size)
    *size = file.size();

  size_t counter = 0;
  while (file.available() && counter++ < *size)
    data[counter] = static_cast<char>(file.read());
  
  file.close();
}

// Interprets file as a CSV of bytes, returns the raw bytes
std::vector<uint8_t> FileHandler::ReadCsvBytes(const char* file_name) {
  size_t temp_len = 0, data_len = FileSize(file_name);
  char data[data_len];  // Char data
  std::vector<uint8_t> out_buffer;  // Output buffer
  String temp_buffer = ""; // Temporary buffer for char data being interpreted

  ReadFile(file_name, data, &data_len);

  out_buffer.reserve(data_len / 3);   // A conservative guess at the appropriate vector size to avoid expensive reallocating
  
  for (size_t copy_len = 1; copy_len < data_len; copy_len++) {  // Start reading at [1] to avoid first garbage digit
    if (temp_buffer.length() && (data[copy_len] == ',' || data[copy_len] == ' ')) { // Commas and spaces act as delimiters
      printf("Adding %s to list\n", temp_buffer.c_str());
      out_buffer.emplace_back(std::atoi(temp_buffer.c_str()));
      temp_buffer = "";
    } else {  // Assume everything else is numbers, which is lazy, but works for now
      temp_buffer += data[copy_len];
    }
  }

  return out_buffer;
}

size_t FileHandler::FileSize(const char* file_name) {
  printf("Trying to open file name %s\n", file_name);
  File file = SPIFFS.open(file_name, FILE_READ);

  if(!file || file.isDirectory()) {
    Serial.println("failed to open file for checking size");
    return 0;
  }
  size_t out = file.size();
  file.close();

  return out;
}

void FileHandler::WriteFile(const char * file_name, const char * message) {
    printf("Trying to open file name %s\n", file_name);
    File file = SPIFFS.open(file_name, FILE_WRITE);
    if(!file){
      Serial.println("failed to open file for writing");
      return;
    }
    if(!file.print(message)){
      Serial.println("write failed");
    }
    file.close();
}

void FileHandler::AppendFile(const char * file_name, const char * message) {
  File file = SPIFFS.open(file_name, FILE_APPEND);
  if(!file){
    Serial.println("failed to open file for appending");
    return;
  }
  if(!file.print(message)){
    Serial.println("append failed");
  }
  file.close();
}

void FileHandler::RenameFile(const char * file_name1, const char * new_name) {
  if (!SPIFFS.rename(file_name1, new_name)) {
    Serial.println("rename failed");
  }
}

void FileHandler::DeleteFile(const char * file_name) {
  if(!SPIFFS.remove(file_name)){
    Serial.println("delete failed");
  }
}

void FileHandler::listDir(const char * dirname, uint8_t levels){
  fs::FS fs = SPIFFS;
    Serial.printf("Listing directory: %s\r\n", dirname);

    File root = fs.open(dirname);
    if(!root){
        Serial.println("failed to open directory");
        return;
    }
    if(!root.isDirectory()){
        Serial.println(" not a directory");
        return;
    }

    File file = root.openNextFile();
    while(file){
        if(file.isDirectory()){
            Serial.print("  DIR : ");
            Serial.println(file.name());
            if(levels){
                listDir(file.name(), levels -1);
            }
        } else {
            Serial.print("  FILE: ");
            Serial.print(file.name());
            Serial.print("\tSIZE: ");
            Serial.println(file.size());
        }
        file = root.openNextFile();
    }
}

}