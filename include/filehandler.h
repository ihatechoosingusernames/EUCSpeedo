#ifndef FILEHANDLER_H_
#define FILEHANDLER_H_

namespace euc {

class FileHandler {
  // if(!SPIFFS.begin(true)){
  //   Serial.println("An Error has occurred while mounting SPIFFS");
  //   return;
  // }

  
  // void readFile(fs::FS &fs, const char * path){
  //   Serial.printf("Reading file: %s\r\n", path);

  //   File file = fs.open(path);
  //   if(!file || file.isDirectory()){
  //       Serial.println("- failed to open file for reading");
  //       return;
  //   }

  //   Serial.println("- read from file:");
  //   while(file.available()){
  //       Serial.write(file.read());
  //   }
  //   file.close();
  // }
};

}

#endif