# KneeTrack Project - SEP6 VIA
## Overview
The KneeTrack project is a solution for reducing knee strain and aims to help people achieve better health for their knees and joints. The project uses IoT technology to monitor the knees' angle and alert the consumer when they have been in an "uneasy" position for an extended period of time.

THE ONLY CONTRIBUTERS ALLOWED UNTIL JUNE, AS IT IS PART OF A SCHOOL PROJECT:

JonasPHenriksen - 330943 Jonas Henriksen 

Twaish - 331188 Dan V. Nguyen

## Features
Angle Monitoring: Checks the knee angle to determine when to alert the user

## Components
Arduino Mega2560: Microcontroller used for processing data and controlling other components.
ADXL345: Accelerometer used for tracking knee rotation

# Installation and Setup

## Software Requirements
- [Arduino IDE](https://www.arduino.cc/en/software/)

## Hardware Setup

### Connect the ADXL345 module:
- Connect the GND pin to the GND port on the Arduino.
- Connect the VCC pin to the 3.3V port on the Arduino.
- Connect the SDA pin to the PD1 port on the Arduino. 
- Connect the SCL pin to the PD0 port on the Arduino.

## Download the Firmware:

Clone the repository
```bash
git clone https://github.com/Twaish/KneeTrack.git
```

Use Arduino IDE to flash the code into the Arduino.

<!--
Obtain the firmware code from the project's release.
Extract the folder.
Open the folder and execute the exe file.
-->
