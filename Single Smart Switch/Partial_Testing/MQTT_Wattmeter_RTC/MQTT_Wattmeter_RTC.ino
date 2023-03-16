#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include "ACS712.h"
#include <Filters.h>
#define TRIGGER_PIN 0
#include "RTClib.h"
#include "Wire.h"

// ------------ RTC DS3231 ------------
RTC_DS3231 rtc;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

// ------------ SENSOR TEGANGAN (ZMPT101B) & SENSOR ARUS (ACS712) ------------
float testFrequency = 50; // signal frequency (Hz)
float windowLength = 40.0 / testFrequency; // how long to average the signal, for statistist
int vSensor = 0;
float intercept = 0; // adjust untuk kalibrasi
float slope = 0.0127; // adjust untuk kalibrasi
//0.0127
//float slope = 0.017;
float voltageSensor;
unsigned long printPeriod = 1000; //Refresh rate
unsigned long previousMillis = 0;
ACS712 currentSensor(ACS712_20A, 35);

// ------------ WIFI ------------
WiFiClient espClient;
const char* ssid = "Jake Sim";
const char* password = "falia2811";

// ------------ MQTT (HiveMQ) ------------
const char* mqtt_server = "broker.hivemq.com";
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

// ------------ RELAY (Venjet) ------------
const int relay = 27;

// ------------------------------------------------------------------------------------

void setup() {
  Serial.begin(115200);
  // Wifi
  setup_wifi();
  // MQTT HiveMQ
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  // Relay (Venjet)
  pinMode(relay, OUTPUT);
  // ACS712 Calibration
  currentSensor.setZeroPoint(1879);
  // RTC
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1) delay(10);
  }

  if (rtc.lostPower()) {
    Serial.println("RTC lost power, let's set the time!");
    // When time needs to be set on a new device, or after a power loss, the
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
}

// ------------------------------------------------------------------------------------

// ------------ WIFI ------------
void setup_wifi() {
  delay(10);
  // Start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

// ------------ MQTT (HiveMQ) ------------
void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;

  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  // Feel free to add more if statements to control more GPIOs with MQTT

  // If a message is received on the topic esp32/output, you check if the message is either "on" or "off".
  // Changes the output state according to the message
  if (String(topic) == "/venambak/admin/control") {
    Serial.print("Changing output to ");
    if (messageTemp == "1") {
      Serial.println("ON");
      digitalWrite(relay, HIGH);
    }
    else if (messageTemp == "0") {
      Serial.println("OFF");
      digitalWrite(relay, LOW);
    }
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
      // Subscribe to receive ON/OFF command
      client.subscribe("/venambak/admin/control");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  RunningStatistics inputStats;
  inputStats.setWindowSecs( windowLength );
  while ( true ) {

    // ------------ RTC DS3231 ------------
    DateTime now = rtc.now();
    // year/month/date (day) hour:minute:second
    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(" (");
    Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
    Serial.print(") ");
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println();

    // ------------ SENSOR TEGANGAN (ZMPT101B) & SENSOR ARUS (ACS712) ------------
    vSensor = analogRead(34); // read the analog in value:
    inputStats.input(vSensor); // log to Stats function

    if ((unsigned long)(millis() - previousMillis) >= printPeriod) {
      previousMillis = millis(); // update time every second

      voltageSensor = intercept + slope * inputStats.sigma(); //Calibartions for offset and amplitude
      voltageSensor = voltageSensor * (49.3231);             //Further calibrations for the amplitude

      // We use 230V because it is the common standard in European countries
      // Change to your local, if necessary
      float U = voltageSensor;

      // To measure current we need to know the frequency of current
      // By default 50Hz is used, but you can specify desired frequency
      // as first argument to getCurrentAC() method, if necessary
      float I = (currentSensor.getCurrentAC()) / 10;

      if (U < 100) {
        U = 0;
        I = 0;
      }

      // To calculate the power we need voltage multiplied by current
      float P = U * I;

      Serial.println(String("V = ") + U + " V");
      Serial.println(String("I = ") + I + " A");
      Serial.println(String("P = ") + P + " Watts");
      Serial.println("");

      // ------------ MQTT ------------
      // Convert the value to a char array
      char PString[100];
      dtostrf(P, 8, 2, PString);
      //      Serial.print("Humidity: ");
      //      Serial.println(humString);
      client.publish("/venambak/admin/monitor", PString);

      // Convert the value to a char array
      //      char humString[100];
      //      dtostrf(humidity, 8, 2, humString);
      //      Serial.print("Humidity: ");
      //      Serial.println(humString);
      //      client.publish("esp32/humidity", humString);

      delay(3000);
    }
  }
}
