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
    Serial.println("error reading from slave");
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
