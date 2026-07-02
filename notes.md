# Branch Notes

This file describes the purpose and planned content of each branch in this repository.

---

## `main`
The production-ready branch. Contains only stable, reviewed code. All feature branches are eventually merged here after review.

---

## `dev`
The integration branch for ongoing development work. Used to combine feature branches and test them together before merging into `main`.

**Planned content:**
- General development notes
- Integration tests
- Configuration files shared across modules

---

## `sensor-module`
Dedicated branch for all sensor-related IoT code and experiments.

**Planned content:**
- Code for reading data from sensors (temperature, humidity, light, etc.)
- Sensor calibration scripts
- Data parsing and processing logic
- Example sketches for DHT11, LDR, ultrasonic sensors, etc.

---

## `wireless-module`
Dedicated branch for wireless communication and IoT connectivity.

**Planned content:**
- Wi-Fi and Bluetooth communication code
- MQTT protocol implementation
- Code for sending sensor data to the cloud (e.g., ThingSpeak, Firebase)
- Example sketches for ESP8266/ESP32 wireless connectivity

---

*Created as part of the IIT Jammu IoT Summer School 2026 - Week 1 Assignment.*
