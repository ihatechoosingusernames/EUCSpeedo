#include "filehandler.h"

#include "SPIFFS.h"

#include "logging.h"

namespace euc {

FileHandler::FileHandler() {
  if (!SPIFFS.begin()) {
    LOG_DEBUG("An Error has occurred while mounting SPIFFS");
  }
}

FileHandler::~FileHandler() {
  SPIFFS.end();
}

void FileHandler::ReadFile(const char * path, char data[], size_t* size) {
  LOG_DEBUG_ARGS("Opening file %s", path);
  File file = SPIFFS.open(path, FILE_READ);

  if(!file || file.isDirectory()){
    LOG_DEBUG("failed to open file for reading");
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
  size_t data_len = FileSize(file_name);
  char data[data_len];  // Char data
  std::vector<uint8_t> out_buffer;  // Output buffer
  String temp_buffer = ""; // Temporary buffer for char data being interpreted

  ReadFile(file_name, data, &data_len);

  out_buffer.reserve(data_len / 3);   // A conservative guess at the appropriate vector size to avoid expensive reallocating
  
  for (size_t copy_len = 1; copy_len < data_len; copy_len++) {  // Start reading at [1] to avoid first garbage digit
    if (temp_buffer.length() && (data[copy_len] == ',' || data[copy_len] == ' ')) { // Commas and spaces act as delimiters
      out_buffer.emplace_back(std::atoi(temp_buffer.c_str()));
      temp_buffer = "";
    } else {  // Assume everything else is numbers, which is lazy, but works for now
      temp_buffer += data[copy_len];
    }
  }

  return out_buffer;
}

size_t FileHandler::FileSize(const char* file_name) {
  LOG_DEBUG_ARGS("Trying to open file name %s starting with %c", file_name, file_name[0]);
  File file = SPIFFS.open(file_name);

  if(!file || file.isDirectory()) {
    LOG_DEBUG_ARGS("failed to open file %s for checking size", file_name);
    return 0;
  }
  size_t out = file.size();
  file.close();

  return out;
}

void FileHandler::WriteFile(const char * file_name, const char * message) {
    LOG_DEBUG_ARGS("Trying to open file name %s starting with %c", file_name, file_name[0]);
    File file = SPIFFS.open(file_name, FILE_WRITE);
    if(!file){
      LOG_DEBUG("failed to open file for writing");
      return;
    }
    if(!file.print(message)){
      LOG_DEBUG("write failed");
    }
    file.close();
}

void FileHandler::AppendFile(const char * file_name, const char * message) {
  File file = SPIFFS.open(file_name, FILE_APPEND);
  if(!file){
    LOG_DEBUG("failed to open file for appending");
    return;
  }
  if(!file.print(message)){
    LOG_DEBUG("append failed");
  }
  file.close();
}

void FileHandler::RenameFile(const char * file_name1, const char * new_name) {
  if (!SPIFFS.rename(file_name1, new_name)) {
    LOG_DEBUG("rename failed");
  }
}

void FileHandler::DeleteFile(const char * file_name) {
  if(!SPIFFS.remove(file_name)){
    LOG_DEBUG("delete failed");
  }
}

void FileHandler::listDir(const char * dirname, uint8_t levels){
  fs::FS fs = SPIFFS;
  LOG_DEBUG_ARGS("Listing directory: %s", dirname);

  File root = fs.open(dirname);
  if(!root){
      LOG_DEBUG("failed to open directory");
      return;
  }
  if(!root.isDirectory()){
      LOG_DEBUG("not a directory");
      return;
  }

  File file = root.openNextFile();
  while(file){
    if(file.isDirectory()){
      LOG_DEBUG_ARGS("  DIR : %s", file.name());
      if(levels){
          listDir(file.name(), levels -1);
      }
    } else {
      LOG_DEBUG_ARGS("  FILE: %s\tSIZE: %d", file.name(), file.size());
    }
    file = root.openNextFile();
  }

  file.close();
}

}