# Sunray_SBUS_AddOn
Sunray remote control via SBUS (e.g. FrSky)

With this add-on for Alexander Grau's Ardumower Sunray Software (https://github.com/Ardumower/Sunray) you can add manual remote control to your Alfred (and Ardumower?).

An SBUS receiver (e.g. FrSKy) is used to receive the remote control data and an ESP32 Dev Board is used for post-processing and converting the data to an I²C interface.

There are three operation modes which can be choosen from the rc transmitter:
1. Automatic mode as usual
2. Manual movement control without mow motor
3. Manual movement control with running mow motor
   
If a failsafe occurs or if the RC transmitter is switched off, all motors are stopped and the usual automatic mode switches on again.
