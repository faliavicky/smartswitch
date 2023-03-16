/*
  To check the SPI communication protocol with the ESP32 using Arduino IDE.
  SPI Interface:
  -> MISO: Master In Slave Out | GPIO 19
  -> MOSI: Master Out Slave In | GPIO 23
  -> SCK: Serial Clock | GPIO 18
  -> CS/SS: Chip Select (used to select the device when multiple peripherals 
     are used on the same SPI bus) | GPIO 5
*/

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.print("MOSI: ");
  Serial.println(MOSI);
  Serial.print("MISO: ");
  Serial.println(MISO);
  Serial.print("SCK: ");
  Serial.println(SCK);
  Serial.print("SS: ");
  Serial.println(SS);
}

void loop() {
  // put your main code here, to run repeatedly:
}
