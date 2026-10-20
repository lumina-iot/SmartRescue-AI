
# SmartRescue AI – Accident Detection and Alert System

## Overview

SmartRescue AI is an IoT-based accident detection and alert prototype developed using an ESP32 microcontroller and simulated in Wokwi. It uses an MPU6050 accelerometer and gyroscope sensor to monitor motion and identify potential high-impact events.

When a potential impact is detected, or the manual test button is pressed, the system activates a local LED and buzzer alert. It can also send accident-status information, acceleration readings, timestamps, and simulated location details to a Blynk IoT mobile dashboard.

This project demonstrates how embedded systems, IoT connectivity, and mobile monitoring can be combined to support faster incident reporting.

## Objectives

- Monitor motion using an MPU6050 sensor.
- Demonstrate potential impact detection.
- Provide local alerts through an LED and buzzer.
- Display accident status and sensor readings through Blynk IoT.
- Show timestamp and simulated location information.
- Explore the foundation for future AI-based accident analysis.

## Technologies Used

- ESP32
- MPU6050 accelerometer and gyroscope
- Wokwi online simulator
- Arduino C++
- Blynk IoT mobile application and cloud
- Wi-Fi connectivity
- NTP-based timestamp synchronisation
- Google Maps URL using sample coordinates

## Hardware and Simulation Components

| Component | Purpose |
|---|---|
| ESP32 | Main controller and Wi-Fi connectivity |
| MPU6050 | Motion and acceleration measurements |
| LED | Visual alert |
| Buzzer | Audible alert |
| Push button | Manual alert testing |
| Wokwi | Circuit simulation |
| Blynk IoT | Remote monitoring and event notifications |

## Pin Connections

| Component | ESP32 connection |
|---|---|
| MPU6050 VCC | 3V3 |
| MPU6050 GND | GND |
| MPU6050 SDA | GPIO 21 |
| MPU6050 SCL | GPIO 22 |
| Push button | GPIO 18 and GND |
| LED | GPIO 23 through a 220-ohm resistor |
| Buzzer positive | GPIO 19 |
| Buzzer negative | GND |

## Blynk Dashboard

The dashboard uses the following virtual datastreams:

| Virtual pin | Data |
|---|---|
| V0 | Accident status (0 or 1) |
| V1 | Acceleration |
| V2 | Status text |
| V3 | Alert count |
| V4 | Timestamp |
| V5 | Simulated location coordinates |
| V6 | Google Maps URL |

Create the corresponding datastreams in your own Blynk template before running the project.

## Working Principle

1. The ESP32 initialises the MPU6050 sensor and connects to Wi-Fi.
2. The sensor measures acceleration along three axes.
3. The program calculates the acceleration magnitude and compares it with a configured threshold.
4. When a potential impact is detected, the LED and buzzer provide a local alert.
5. The program updates the Blynk dashboard with the alert status, acceleration, alert count, timestamp, and simulated location.
6. The manual push button allows the alert workflow to be tested without creating an actual impact.

## Software Setup

1. Open the project in Wokwi and use an ESP32 board.
2. Install the required libraries: `Adafruit MPU6050`, `Adafruit Unified Sensor`, and `Adafruit BusIO`.
3. Configure a Blynk IoT template and device.
4. Create the required V0–V6 datastreams and an event named `accident_alert`.
5. Add your own Blynk device token securely in the local code.
6. Run the simulation and open the Serial Monitor at 115200 baud.
7. Press the manual test button and check the Blynk dashboard.

## Project Status

**Prototype / Simulation**

The project demonstrates sensor monitoring, local alerts, and remote dashboard updates in simulation.

## Limitations

- The location coordinates are simulated and are not obtained from a physical GPS receiver.
- The acceleration threshold is a demonstration and is not validated for real accident detection.
- The timestamp requires internet time synchronisation.
- The current prototype does not implement a trained AI model. AI-based anomaly analysis and automated incident classification are possible future enhancements.
- The system has not been validated for real-world emergency use.

## Future Enhancements

- Integrate a GPS module or a supported live-location source.
- Develop an AI model for motion-anomaly classification.
- Add emergency contact notifications.
- Improve false-positive handling and sensor calibration.
- Add secure data storage and incident history.
- Test with physical hardware under controlled conditions.

## Author

**Lumina R**  
IoT Engineering Student

## Disclaimer

SmartRescue AI is an educational prototype intended for learning and demonstration. It must not be relied upon as a real emergency detection or rescue system.
