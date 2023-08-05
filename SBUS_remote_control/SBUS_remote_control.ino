/*
  FrSky X4R receiver
  ESP32 Dev Board
  connections:
  3,3V to (+)
  GND to (-)
  D16 to SBUS
*/
// EXPO curve development see here: https://forum.arduino.cc/t/expo-einstellung-rc-mathematik/1150995

#include <Wire.h>
#include "sbus.h"             // from https://github.com/bolderflight/sbus
bfs::SbusRx sbus_rx(&Serial1, 16, 17, true);  // declaration of rx and tx pins between FrSky receiver and ESP32 dev board, we only need the rx
bfs::SbusData data;

short throttle_min = 172;     // determined value on my setup for throttle minimum position
short throttle_mid = 805;     // determined value on my setup for throttle middle position
short throttle_max = 1811;    // determined value on my setup for throttle maximum position
short steering_left = 172;    // determined value on my setup for steering left position
short steering_mid = 990;     // determined value on my setup for steering middle position
short steering_right = 1811;  // determined value on my setup for steering right position
short mow_full = 1811;        // determined value on my setup for mow motor full power position
short mow_off = 172;          // determined value on my setup for mow motor off position
byte throttle_deadband = 20;  // deadband for throttle middle-position to avoid slow movement around middle-position, and both end positions
byte steering_deadband = 20;  // deadband for steering middle-position to avoid slow movement around middle-position, and both end positions
short throttle_stick;         // input throttle stick
short steering_stick;         // input steering stick
short mow_pot;                // input potentiometer mow motor
short throttle;               // throttle value
short steering;               // steering value
short expo_throttle = 0;      // expects a value between -100 and +100. recommend value= 0, 0=linear proportional curve, -100=progressive curve in the middle, +100=degressive curve in the middle
short expo_steering = 0;     // expects a value between -100 and +100. recommend value=30, 0=linear proportional curve, -100=progressive curve in the middle, +100=degressive curve in the middle
short throttlecurve[2048];    // throttlecurve points array to save calculation time in loop
short steeringcurve[2048];    // steeringcurve points array to save calculation time in loop
short throttlewithexpo;       // throttle value mixed with expo
short steeringwithexpo;       // steering value mixed with expo
short motor_mow;              // output for right motor
short motor_left;             // output for left motor
short motor_right;            // output for right motor


short expo_curve(short x, short expo) {                // expects input values from -1023 to +1023 and outputs values from -1023 to +1023. The 'only' change is the proportionality factor between input and output
  short y;                                             // EXPO curve development see here: https://forum.arduino.cc/t/expo-einstellung-rc-mathematik/1150995
  if (expo < 0) {
    if (x < 0) {
      y = (abs(expo / 100.) / pow(1023, 2)) * pow(1023 - abs(x), 3) + (1 - abs(expo / 100.)) * (1023 - abs(x)) - 1023;
    }
    else {
      y = 1023 - ((abs(expo / 100.) / pow(1023, 2)) * pow(1023 - abs(x), 3) + (1 - abs(expo / 100.)) * (1023 - abs(x)));
    }
  } else {
    y = (expo / 100. / pow(1023, 2)) * pow(x, 3) + (1 - expo / 100.) * x;
  }
  return y;
}


void setup() {
  Serial.begin(115200);
  while (!Serial) {}
  sbus_rx.Begin();
  for (int x = -1023; x <= 1023; x++) {             // calculates throttlecurve and steeringcurve and store all curvepoints in arrays.
    throttlecurve[x + 1023] = expo_curve(x, expo_throttle);
    steeringcurve[x + 1023] = expo_curve(x, expo_steering);
  }
  // ----------- ADDED ------------------------------
  Wire.begin(0x55); // Initialize I2C (Slave Mode: address=0x55 )
  Wire.onRequest(I2C_TxHandler);
}

// send short integer over I2C
void sendShort(short value){
  Wire.write((byte)(value >> 8)); // send highest 8 bits
  Wire.write((byte)(value & 0xFF));  // send lowest 8 bits
}

// I2C slave callback
void I2C_TxHandler(void)
{
  sendShort(throttle);
  sendShort(steering);
  sendShort(motor_left);
  sendShort(motor_right);
  sendShort(motor_mow);
}

// --------------ADDED END----------------------------

void loop () {
  if (sbus_rx.Read()) {
    data = sbus_rx.data();
    // --------------------throttle stick--------------------
    throttle_stick = data.ch[0];
    if (throttle_stick > throttle_mid + throttle_deadband / 2) {
      throttle = (map(throttle_stick, throttle_mid + throttle_deadband / 2, throttle_max, 0, 1023));
    } else {
      if (throttle_stick < throttle_mid - throttle_deadband / 2) {
        throttle = (map(throttle_stick, throttle_mid - throttle_deadband / 2, throttle_min, 0, -1023));
      } else {
        throttle = 0;
      }
    };

    // --------------------steering stick--------------------
    steering_stick = data.ch[1];
    if (steering_stick > steering_mid + steering_deadband / 2) {
      steering = (map(steering_stick, steering_mid + steering_deadband / 2, steering_right, 0, 1023));
    } else {
      if (steering_stick < steering_mid - steering_deadband / 2) {
        steering = (map(steering_stick, steering_mid - steering_deadband / 2, steering_left, 0, -1023));
      } else {
        steering = 0;
      }
    };

    // --------------------mowmotor potentiometer--------------------
    mow_pot = data.ch[4];
    motor_mow = (map(mow_pot, mow_off, mow_full, 0, 1023));

    // --------------------failsafe--------------------
    if (data.failsafe > 0) {
      Serial.print("failsafe");    // if a failsafe is recognized by the FrSky receiver
      throttle = 0;
      steering = 0;
      motor_left = 0;
      motor_right = 0;
      motor_mow = 0;
    }
    else {
      Serial.print("running");    // if no failsafe is recognized by the FrSky receiver
    }

    // --------------------throttle and steering with expo--------------------
    throttlewithexpo = throttlecurve[throttle + 1023];
    steeringwithexpo = steeringcurve[steering + 1023];

    // --------------------cross-mixer left motor--------------------
    if (throttlewithexpo + steeringwithexpo > 0) {                 // if the sum of throttlewithexpo and steeringwithexpo is forward
      if (throttlewithexpo + steeringwithexpo < 1023) {            // if the sum of throttlewithexpo and steeringwithexpo is less the full forward
        motor_left = throttlewithexpo + steeringwithexpo;          // then motor_left = throttlewithexpo + steeringwithexpo
      } else {
        motor_left = 1023;                                         // else motor_left =  full forward
      }
    } else {                                                       // else the sum of throttlewithexpo and steeringwithexpo is backward
      if (throttlewithexpo + steeringwithexpo > -1023) {           // if the sum throttlewithexpo and steeringwithexpo is less then full backwards
        motor_left = throttlewithexpo + steeringwithexpo;          // then motor_left = throttlewithexpo + steeringwithexpo
      } else {
        motor_left = -1023;                                        // else motor_left = full backwards
      }
    }

    // --------------------cross-mixer right motor--------------------
    if (throttlewithexpo - steeringwithexpo > 0) {                 // if the difference between throttlewithexpo and steeringwithexpo is forward
      if (throttlewithexpo - steeringwithexpo < 1023) {            // if the difference between throttlewithexpo and steeringwithexpo is less the full forward
        motor_right = throttlewithexpo - steeringwithexpo;         // then motor_right = throttlewithexpo - steeringwithexpo
      } else {
        motor_right = 1023;                                        // else motor_right =  full forward
      }
    } else {                                                       // else the difference between throttlewithexpo and steeringwithexpo is backward
      if (throttlewithexpo - steeringwithexpo > -1023) {           // if the difference between throttlewithexpo and steeringwithexpo is less then full backwards
        motor_right = throttlewithexpo - steeringwithexpo;         // then motor_right = throttlewithexpo - steeringwithexpo
      } else {
        motor_right = -1023;                                       // else motor_right = full backwards
      }
    }

    // --------------------send values on I²C--------------------
    Serial.print("\t");
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
}
