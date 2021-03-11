#include "filehandler.h"

#include "SPIFFS.h"

namespace euc {

FileHandler::FileHandler() {
  if (!SPIFFS.begin(true)) {
    Serial.println("An Error has occurred while mounting SPIFFS");
  }
}

uint8_t* FileHandler::ReadFile(const char * path, size_t* size) {
  File file = SPIFFS.open(path);

  *size = file.size();
  uint8_t out[*size];

  if(!file || file.isDirectory()){
    Serial.println("- failed to open file for reading");
    delete out;
    *size = 0;
    return out;
  }

  while(file.available()){
    file.read(out, *size);
  }
  file.close();

  return out;
}

}