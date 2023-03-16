#include "FS.h"
#include "SD.h"
#include "SPI.h"

#define SD_CS 5

void writeFile(fs::FS &fs, const char * path, const char * message){
    Serial.printf("Writing file: %s\n", path);

    File file = fs.open(path, FILE_WRITE);
    if(!file){
        Serial.println("Failed to open file for writing");
        return;
    }
    if(file.print(message)){
        Serial.println("File written");
    } else {
        Serial.println("Write failed");
    }
    file.close();
}

void setup(){
  Serial.begin(9600);
  SD.begin(SD_CS);
  if (!SD.begin(SD_CS)) {
    Serial.println("Gagal Memuat Kartu SD");
    return;
  }
  uint8_t cardType = SD.cardType();
  if (cardType == CARD_NONE) {
    Serial.println("Tidak Ada Kartu SD");
    return;
  }
  Serial.println("Menginisialisasi kartu SD...");
  
  writeFile(SD, "/RobotikID.txt", "Hai, Ini Robotik Indonesia ");
  /*
  Menyimpan Tulisan atau String "Hai, Ini Robotik Indonesia "
  pada file yang bernama RobotikID.txt
  */ 
}

void loop(){
}
