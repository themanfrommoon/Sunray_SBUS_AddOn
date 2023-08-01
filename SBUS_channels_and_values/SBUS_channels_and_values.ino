/*
  Brian R Taylor
  brian.taylor@bolderflight.com

  Copyright (c) 2022 Bolder Flight Systems Inc

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the “Software”), to
  deal in the Software without restriction, including without limitation the
  rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
  sell copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
  IN THE SOFTWARE.
*/

/*
  themanfrommoon Christian Wulff 01.08.2023:

  Slightly modified code from the original Bolder Flight Systems SBUS library example "sbus_example.ino"
  
  Compiled successfully with:
  Arduino IDE 1.8.19
  Espressif Systems board manager "esp32" Version 2.0.11
  Board "ESP32 Dev Module"
  Bolder Flight Systems SBUS Library Version 8.1.4

  used Hardware:
  FrSky X4R Receiver
  ESP32 Dev Module
  
  Connections:
  FrSky X4R Receiver ----- ESP32 Dev Module
              (-)    -----    GND
              (+)    -----    3V3
             (SB)    -----    G16

  FrSky Protocol Mode: ACCST D16

Description:
This sketch reads the SBUS protocol from the FrSky receiver and prints all 16 analog and 2 digital channels to the serial monitor.
It is used to check the connection, which channels you like to use and which values these channels have in min, mid and max position.
These information are needed for the main sketch.

Refer to the original author Brian R Taylor which hardware can be used: https://github.com/bolderflight/sbus
ATTENTION: There is also a hint, that there may be issues with installing the library by the library manager in the Arduino IDE.
It is recommend to install the library manual (copy the library into the library path manually)

The lines for SBUS transmission has been removed because they are not needed here.

ATTENTION: If the upload doies not work you may have to press the BOOT button of the ESP32 Dev module during the upload!
*/

#include "sbus.h"

/* SBUS object, reading SBUS */
bfs::SbusRx sbus_rx(&Serial1, 16, 17, true);       // this line is specially adapted for ESP32 use!!!

/* SBUS data */
bfs::SbusData data;

void setup() {
  /* Serial to display data */
  Serial.begin(115200);
  while (!Serial) {}
  /* Begin the SBUS communication */
  sbus_rx.Begin();
}

void loop () {
  if (sbus_rx.Read()) {
    /* Grab the received data */
    data = sbus_rx.data();
    /* Display the received data */
    for (int8_t i = 0; i < data.NUM_CH; i++) {
      Serial.print(data.ch[i]);
      Serial.print("\t");
    }
    /* Display lost frames and failsafe data */
    Serial.print(data.lost_frame);
    Serial.print("\t");
    Serial.println(data.failsafe);
  }
}
