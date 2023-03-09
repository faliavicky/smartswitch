# Smart Switch
A Smart Switch project for AC load power monitoring, data logging with micro SD, scheduling from mobile app with Real-Time Clock, and remote on/off load control using ESP32 and MQTT.

### To-Do List
- [x] <b>Single Output Smart Switch</b>
   - [x] Wattmeter
      - [x] Voltage
      - [x] Current
   - [x] MQTT
   - [x] Publish wattmeter to MQTT
   - [x] On/Off
   - [x] On/Off with MQTT
   - [x] RTC
      - [ ] Scheduling with RTC
      - [ ] Scheduling with RTC and MQTT
   - [x] Data Logging
      - [x] RTC Data Logging
      - [ ] Upload Data Logging after ESP32 connected to Wi-Fi
      - [ ] Delete previous data after uploading
- [ ] <b>Multiple Outputs Smart Switch 1 Phase</b>
- [ ] <b>Multiple Outputs Smart Switch 3 Phase</b>

---

### Sensors used

| Sensors  | Type | Capacity |
| ------------- | ------------- | ------------- |
| Voltage Sensor  | ZMPT101B  | 220V/380V |
| Current Sensor  | ACS712  | 20A |
| RTC  | DS3231  |  |
| Micro SD Module  |  |  |

---

### Components used

| Components |
| ------------- |
| ESP32 Devkit V1 |
| Relay   1 channel 20A |
| DS3231  |
| Micro SD Module |
