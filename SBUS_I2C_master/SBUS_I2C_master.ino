/*
   original I²C code by greymfm Alexander Grau 31.07.2023
*/

/*
  themanfrommoon Christian Wulff 01.08.2023:

  Compiled successfully with:
  Arduino IDE 1.8.19
  ESP8266 Community board manager "esp8266" Version 3.1.2
  Board "NodeMCU 0.9 (ESP-12 Module)"

  used Hardware:
  nodeMCU ESP8266 as I²C Master
  ESP32 Dev Module as I²C Slave

  Connections:
  ESP32 Dev Module ----- NodeMCU ESP8266
            GND    -----    GND
            G21    -----    D2    (SDA)
            G22    -----    D1    (SCL)

  Description:
  This sketch is used for testing the I²C functionality between the ESP32 Dev Module and NodeMCU ESP8266.
  The ESP32 sends the RC receiver data to the I²C bus and the ESP8266 prints the information on the serial monitor
*/

#include <Wire.h>
short throttle;
short steering;
short motor_left;
short motor_right;
short motor_mow;

void setup() {
  Serial.begin(115200);
  Wire.begin(); // Initialize I2C (Master Mode: address is optional)
}

// Read data from slave
short readShort() {
  if (!Wire.available()) return 0;
  byte highByte = Wire.read();
  if (!Wire.available()) return 0;
  byte lowByte = Wire.read();
  short value = (highByte << 8) | lowByte;
  return value;
}

void requestDataFromSlave() {

  // Request From Slave @ 0x55, Data Length = 10 Bytes
  if (Wire.requestFrom(0x55, 10) != 10) {
    Serial.print("error reading from slave");
    return;
  };

  throttle = readShort();
  steering = readShort();
  motor_left = readShort();
  motor_right = readShort();
  motor_mow = readShort();
}

void loop() {
  requestDataFromSlave();
  Serial.print("\t");
  Serial.print(throttle);
  Serial.print("\t");
  Serial.print(steering);
  Serial.print("\t");
  Serial.print(motor_left);
  Serial.print("\t");
  Serial.print(motor_right);
  Serial.print("\t");
  Serial.println(motor_mow);
}
