#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include "ACS712.h"
#include <Filters.h>
#define TRIGGER_PIN 0
#include <ArduinoJson.h>

// ---- SENSOR TEGANGAN & SENSOR ARUS
float testFrequency = 50;                     // signal frequency (Hz)
float windowLength = 40.0 / testFrequency;   // how long to average the signal, for statistist
int vSensor = 0;
float intercept = 0;  // adjust untuk kalibrasi
float slope = 0.0127;   // adjust untuk kalibrasi
//0.0127
//float slope = 0.017;
float voltageSensor;
unsigned long printPeriod = 1000;     //Refresh rate
unsigned long previousMillis = 0;
ACS712 currentSensor(ACS712_20A, 35);

// ---- WIFI
const char* ssid = "Jake Sim";
const char* password = "falia2811";

// ---- MQTT
const char* mqtt_server = "broker.hivemq.com";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

//float temperature = 0;
//float humidity = 0;

// ----- RELAY
const int relay = 27;

void setup() {
  Serial.begin(115200);
  // default settings
  // (you can also pass in a Wire library object like &Wire2)

  // WIFI
  setup_wifi();

  // MQTT
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  // RELAY
  pinMode(relay, OUTPUT);
  // ACS712
  currentSensor.setZeroPoint(1879);
}

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
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
      // Subscribe
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
    vSensor = analogRead(34);                // read the analog in value:
    inputStats.input(vSensor);                   // log to Stats function

    if ((unsigned long)(millis() - previousMillis) >= printPeriod) {
      previousMillis = millis();                // update time every second

      voltageSensor = intercept + slope * inputStats.sigma(); //Calibartions for offset and amplitude
      voltageSensor = voltageSensor * (49.3231);             //Further calibrations for the amplitude

      // We use 230V because it is the common standard in European countries
      // Change to your local, if necessary
      float U = voltageSensor;

      // To measure current we need to know the frequency of current
      // By default 50Hz is used, but you can specify desired frequency
      // as first argument to getCurrentAC() method, if necessary
      float I = (currentSensor.getCurrentAC()) / 10;

      //      if (U < 100) {
      //        U = 0;
      //        I = 0;
      //      }

      // To calculate the power we need voltage multiplied by current
      float P = U * I;

      Serial.println(String("V = ") + U + " V");
      Serial.println(String("I = ") + I + " A");
      Serial.println(String("P = ") + P + " Watts");


      while (!Serial) continue;
      StaticJsonDocument<200> sensor1;
      JsonArray daya = sensor1.createNestedArray("daya");
      daya.add(P);
      //      JsonArray tegangan = sensor1.createNestedArray("tegangan");
      //      tegangan.add(U);

      serializeJson(sensor1, Serial);
      Serial.println();
      // serializeJsonPretty(sensor1, Serial);

      //PString = String(P);
      //      char PString[100];
      //      dtostrf(P, 8, 2, PString);

      const char* json = sprintf("{\"daya\": %s\"}", PString );
      //      //      const char* json = "{\"daya\":\ + PString \}";

      //Serial.println(json);
      //Serial.println(PString);
      client.publish("/venambak/admin/monitor", json);

      // Convert the value to a char array
      //      char PString[100];
      //      dtostrf(P, 8, 2, PString);
      //      Serial.print("Humidity: ");
      //      Serial.println(humString);
      //      client.publish("/venambak/admin/monitor", PString);

      // Convert the value to a char array
      //      char humString[100];
      //      dtostrf(humidity, 8, 2, humString);
      //      Serial.print("Humidity: ");
      //      Serial.println(humString);
      //      client.publish("esp32/humidity", humString);
    }
  }
}
