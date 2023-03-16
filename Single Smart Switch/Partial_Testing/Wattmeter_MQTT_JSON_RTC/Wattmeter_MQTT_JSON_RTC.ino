#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include "ACS712.h"
#include <Filters.h>
#define TRIGGER_PIN 0
#include <ArduinoJson.h>
#include "RTClib.h"
//float P = 220;

// ---------- RELAY ----------
int relay = 27;

// ---------- RTC ----------
RTC_DS3231 rtc;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

// -------- WIFI and MQTT --------

const char* ssid = "Jake Sim";
const char* password = "falia2811";
const char* mqtt_server = "broker.hivemq.com";

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE  (50)
char msg[MSG_BUFFER_SIZE];
int value = 0;

// ---- VOLTAGE SENSOR & CURRENT SENSOR ----
float testFrequency = 50; // signal frequency (Hz)
float windowLength = 40.0 / testFrequency;   // how long to average the signal, for statistist
int vSensor = 0;
float intercept = 0; // adjust for calibration
float slope = 0.0127; // adjust for calibration
float voltageSensor;
unsigned long printPeriod = 1000; //Refresh rate
unsigned long previousMillis = 0;
ACS712 currentSensor(ACS712_20A, 35);


void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Getting values from JSON
  StaticJsonDocument<200> doc;
  //char* json = topic;
  DeserializationError error = deserializeJson(doc, (char*)payload);
  bool receive_mode = doc["automatic"];
  bool receive_state = doc["state"];
  Serial.println(receive_state);

  // ON/OFF switch with receive_state
  if (receive_state == 1) {
    digitalWrite(relay, HIGH);   // Turn the switch on
    Serial.println("ON");
  } else {
    digitalWrite(relay, LOW);  // Turn the switch off
    Serial.println("OFF");
    //Serial.println( TYPE_NAME(receive_state));
  }

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP32";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("/venambak/admin/monitor", "hello world");
      // ... and resubscribe
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

void setup() {
  pinMode(relay, OUTPUT); // Initialize the relay pin as an output
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  currentSensor.setZeroPoint(1879); //set current sensor to 0
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
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }
}

void loop() {
  RunningStatistics inputStats;
  inputStats.setWindowSecs( windowLength );
  //while ( true ) {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Voltage Sensor and Current Sensor
  vSensor = analogRead(34); // read the analog in value:
  inputStats.input(vSensor); // log to Stats function

  if ((unsigned long)(millis() - previousMillis) >= printPeriod) {
    previousMillis = millis();                // update time every second

    voltageSensor = intercept + slope * inputStats.sigma(); //Calibartions for offset and amplitude
    voltageSensor = voltageSensor * (49.3231);             //Further calibrations for the amplitude

    // We use 230V because it is the common standard in European countries
    // Change to your local, if necessary (Indonesia : 220V)
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

    // Sending the voltage, current, and power values as JSON
    DynamicJsonDocument root(1024);
    root["arus"] = String(U);
    root["tegangan"] = String(I);
    root["daya"] = String(P);
    char myBuffer[1023];
    serializeJson(root, myBuffer);
    client.publish("/venambak/admin/monitor", myBuffer);
    Serial.println(myBuffer);

    // RTC
    DateTime now = rtc.now();
    // -----> year/month/date (day) hour:minute:second
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
    delay(3000);

    //      unsigned long now = millis();
    //      if (now - lastMsg > 2000) {
    //
    //        //        lastMsg = now;
    //        ++value;
    //        snprintf (msg, MSG_BUFFER_SIZE, "%f", P);
    //        Serial.print("Publish message: ");
    //        Serial.println(msg);
    //        client.publish("/venambak/admin/monitor", msg);
  }

  //}
}
