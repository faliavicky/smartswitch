#include <SD.h>

#define SD_CS 5

File myFile;

void setup() {
  Serial.begin(115200);

  // Initialize the SD card
  if (!SD.begin(SD_CS)) {
    Serial.println("Failed to mount the SD card!");
    return;
  }
  Serial.println("SD card mounted successfully!");

  // Open a file for writing
  myFile = SD.open("/test.txt", FILE_WRITE);

  // Write some data to the file
  if (myFile) {
    myFile.println("Hello from ESP32!");
    myFile.close();
    Serial.println("Data written to file successfully!");
  } else {
    Serial.println("Failed to open file for writing!");
  }
}

void loop() {}
