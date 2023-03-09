
#include <ESP8266WiFi.h>
#include "secrets.h"
#include "ThingSpeak.h" // always include thingspeak header file after other header files and custom macros

#include <String.h>
bool captureSerialRx = false;
bool completeSerialRx = false;

String bufferRx;
String strSuhuTerima;
String strLedTerima;

char ssid[] = "hehehe";   // your network SSID (name)
char pass[] = "12345679";   // your network password
int keyIndex = 0;            // your network key Index number (needed only for WEP)
WiFiClient  client;

unsigned long myChannelNumber = 1375691;
const char * myWriteAPIKey = "HT44BNSB1XZYLDMZ";

String myStatus = "";

void setup() {
  Serial.begin(9600);  // Initialize serial
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo native USB port only
  }

  WiFi.mode(WIFI_STA);
  ThingSpeak.begin(client);  // Initialize ThingSpeak
}

void loop() {
  readData();
  // Connect or reconnect to WiFi
  if (WiFi.status() != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(SECRET_SSID);
    while (WiFi.status() != WL_CONNECTED) {
      WiFi.begin(ssid, pass);  // Connect to WPA/WPA2 network. Change this line if using open or WEP network
      Serial.print(".");
      delay(5000);
    }
    Serial.println("\nConnected.");
  }

  // set the fields with the values
  ThingSpeak.setField(1, strSuhuTerima);
  ThingSpeak.setField(2, strLedTerima);
  //  ThingSpeak.setField(3, number3);
  //  ThingSpeak.setField(4, number4);

  // figure out the status message
  //  if(number1 > number2){
  //    myStatus = String("field1 is greater than field2");
  //  }
  //  else if(number1 < number2){
  //    myStatus = String("field1 is less than field2");
  //  }
  //  else{
  myStatus = String("aman");
  //  }

  // set the status
  ThingSpeak.setStatus(myStatus);

  // write to the ThingSpeak channel
  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  if (x == 200) {
    Serial.println("Channel update successful.");
  }
  else {
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }

  // change the values
  //  number1++;
  //  if(number1 > 99){
  //    number1 = 0;
  //  }
  //  number2 = random(0,100);
  //  number3 = random(0,100);
  //  number4 = random(0,100);

//  delay(1000); // Wait 20 seconds to update the channel again
}

void readData() {
  if (Serial.available()) {
    char rxChar = Serial.read();

    if (rxChar == 'A' && !captureSerialRx) captureSerialRx = true;
    else if (rxChar == 'B' && captureSerialRx) {
      captureSerialRx = false;
      completeSerialRx = true;
    }

    if (captureSerialRx) {
      bufferRx += rxChar;
    }
  }

  if (completeSerialRx) {
    strSuhuTerima = bufferRx.charAt(1);
    strSuhuTerima += bufferRx.charAt(2);
    strSuhuTerima += bufferRx.charAt(3);
    strSuhuTerima += bufferRx.charAt(4);
    strSuhuTerima += bufferRx.charAt(5);

    strLedTerima = bufferRx.charAt(6);
    bufferRx = "";
    completeSerialRx = false;
    Serial.println(strSuhuTerima);
    Serial.println(strLedTerima);
  }
}
