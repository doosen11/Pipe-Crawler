# Pipe-Crawler
This code was written to control the sensors of the Pipe Crawler created for MakeHarvard 2019

The code is written for the Sparkfun ESP32 Thing board, and programmed using the Arduino IDE. 
It initializes two sensors, a CCS811 Air Quality Sensor and a Sparkfun Sonar Sensor breakout board. 
After initializing the sensors, a simple server is created on port 80 on the WiFi, and hosts a website locally. 
The website displays the data being transmitted from the sensors in real time. If the detected distance from the sensors to the pipe surface becomes too large, it indicates an integrity violation in the surface of the pipe. 
