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

#define SerialOutput       // Switch on/off serial monitor output "debugging mode" (uncomment for productive use, only useful for debugging)
#include <Wire.h>
short control_mode;        // control mode: "0"-automatic control, "1"-manual movement control without mow motor, "2"-manual movement control with running mow motor. If a failsafe occurs or if the RC transmitter is switched off, all motors are stopped and the usual automatic mode switches on again.
short motor_left;          // +/-1023 motor left value
short motor_right;         // +/-1023 motor right value

void setup() {
#ifdef SerialOutput       // serial output only active in "debugging mode"
  Serial.begin(115200);   // start serial monitoring
#endif
  Wire.begin();           // Initialize I2C (Master Mode: address is optional)
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

void requestDataFromSlave() {  // Request From Slave @ 0x55, Data Length = 10 Bytes
  if (Wire.requestFrom(0x55, 10) != 10) {
#ifdef SerialOutput             // serial output only active in "debugging mode"
    Serial.print("error reading from slave");
#endif
    return;
  };
  control_mode = readShort();   // first value is control_mode value
  motor_left = readShort();     // second value is motor_left value
  motor_right = readShort();    // third value is motor_right value
}

void loop() {
  requestDataFromSlave();
#ifdef SerialOutput             // serial output only active in "debugging mode"
  Serial.print("\t");
  Serial.print(control_mode);
  Serial.print("\t");
  Serial.print(motor_left);
  Serial.print("\t");
  Serial.println(motor_right);
#endif
}
