# Smart Switch
A Smart Switch system for real-time remote controlling and monitoring AC loads with IoT.
This project is also equipped with data logging using a micro SD card module and scheduling from a mobile app with a real-time clock (RTC) module.
ESP32 is used as the microcontroller with MQTT communication.
The variables that are being monitored are the voltage, current, and watt values.
The loads can be controlled (on/off) by users through a mobile app.

[Partial_Testing](https://github.com/faliavicky/smartswitch/tree/main/Single%20Smart%20Switch/Partial_Testing) consists of each code for different purposes.
[Smart_Switch](https://github.com/faliavicky/smartswitch/tree/main/Single%20Smart%20Switch/Smart_Switch) consists of the Smart Switch system codes.

- [x] [<b>Single Output Smart Switch</b>](https://github.com/faliavicky/smartswitch/tree/main/Single%20Smart%20Switch/Smart_Switch)
   - [x] [Wattmeter](https://github.com/faliavicky/smartswitch/tree/main/Single%20Smart%20Switch/Partial_Testing/Wattmeter)
      - [x] Voltage
      - [x] Current
   - [x] [MQTT](https://github.com/faliavicky/smartswitch/tree/main/Single%20Smart%20Switch/Partial_Testing/MQTT_and_JSON_Serialization)
   - [x] [Publish wattmeter to MQTT](https://github.com/faliavicky/smartswitch/tree/main/Single%20Smart%20Switch/Partial_Testing/MQTT_Wattmeter)
   - [x] On/Off
   - [x] On/Off with MQTT
   - [x] [RTC](https://github.com/faliavicky/smartswitch/tree/main/Single%20Smart%20Switch/Partial_Testing/RTC)
      - [x] Scheduling with RTC
      - [ ] Scheduling with RTC and MQTT
   - [x] [Data Logging](https://github.com/faliavicky/smartswitch/tree/main/Single%20Smart%20Switch/Partial_Testing/MicroSD_Card_Module_Testing)
      - [ ] RTC Data Logging
      - [ ] Upload Data Logging after ESP32 is connected to Wi-Fi
      - [ ] Delete previous data after uploading

### Documentation
![Smart Switch Board](https://github.com/faliavicky/smartswitch/blob/f540cc3d1f1c7f9d2ef203a9e84e48dba2483f8d/Single%20Smart%20Switch/Documentation/Smart-Switch-Board.jpg)
The diagram schematic can be seen [here](https://github.com/faliavicky/smartswitch/tree/main/Single%20Smart%20Switch/Documentation).

### Sensors and Components used

| Sensors  | Type | Capacity |
| ------------- | ------------- | ------------- |
| Voltage Sensor  | ZMPT101B  | 220V/380V |
| Current Sensor  | ACS712  | 20A |

| Components |
| ------------- |
| ESP32 Devkit V1 |
| Relay   1 channel 20A |
| DS3231  |
| Micro SD Module |


### Pins

| Components  | ESP32 Pin |
| ------------- | ------------- |
| Voltage Sensor (Output)  | D34 |
| Current Sensor (Output) | D35 |
| Relay | D27 |

| MicroSD Card Module  | ESP32 Pin |
| ------------- | ------------- |
| GND  | GND |
| VCC  | 3V3 |
| MISO  | D19 |
| MOSI  | D23 |
| SCK  | D18 |
| CS  | D5 |

| RTC DS3231  | ESP32 Pin |
| ------------- | ------------- |
| GND  | GND |
| VCC  | 3V3 |
| SCL  | D22_SCL |
| SDA  | D21_SDA |
