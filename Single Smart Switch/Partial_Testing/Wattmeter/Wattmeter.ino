#include "ACS712.h"
#include <Filters.h>
#define TRIGGER_PIN 0
int relay = 27;

float testFrequency = 50;                     // signal frequency (Hz)
float windowLength = 40.0/testFrequency;     // how long to average the signal, for statistist
int vSensor = 0;
float intercept = 0;  // adjust untuk kalibrasi
float slope = 0.0127;   // adjust untuk kalibrasi
//0.0127
//float slope = 0.017;
float voltageSensor; 
unsigned long printPeriod = 1000;     //Refresh rate
unsigned long previousMillis = 0;

ACS712 currentSensor(ACS712_20A, 35);

void setup() {
  Serial.begin(9600);

  pinMode(relay, OUTPUT);

  // calibrate() method calibrates zero point of sensor,
  // It is not necessary, but may positively affect the accuracy
  // Ensure that no current flows through the sensor at this moment
  // If you are not sure that the current through the sensor will not leak during calibration - comment out this method
  Serial.println("Calibrating... Ensure that no current flows through the sensor at this moment");
  //currentSensor.calibrate();
  currentSensor.setZeroPoint(1879);
  //currentSensor.setSensitivity(0.10);
  Serial.println("Done!");
}

void loop() {
  RunningStatistics inputStats;               
  inputStats.setWindowSecs( windowLength );
  while( true ) {   
    vSensor = analogRead(34);                // read the analog in value:
    inputStats.input(vSensor);                   // log to Stats function
        
    if((unsigned long)(millis() - previousMillis) >= printPeriod) {
      previousMillis = millis();                // update time every second
      
      voltageSensor = intercept + slope * inputStats.sigma(); //Calibartions for offset and amplitude
      voltageSensor= voltageSensor*(49.3231);                //Further calibrations for the amplitude
              
      // We use 230V because it is the common standard in European countries
      // Change to your local, if necessary
      float U = voltageSensor;

      // To measure current we need to know the frequency of current
      // By default 50Hz is used, but you can specify desired frequency
      // as first argument to getCurrentAC() method, if necessary
      float I = (currentSensor.getCurrentAC())/10;
      
//      if(U < 100){
//        U = 0;
//        I = 0;
//        }
        
      // To calculate the power we need voltage multiplied by current
      float P = U * I;
      
      Serial.println(String("V = ") + U + " V");
      Serial.println(String("I = ") + I + " A");      
      Serial.println(String("P = ") + P + " Watts");

      //delay(1000);
      
      if(digitalRead(TRIGGER_PIN)==LOW){
        digitalWrite(relay, HIGH);
        }
       else{
        digitalWrite(relay, LOW);
        }
              
      }
  }

      
  
  
}
