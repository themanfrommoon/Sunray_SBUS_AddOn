# Sunray_SBUS_AddOn
Sunray remote control via SBUS (e.g. FrSky)

![remote_control](docs/remote_control.png)

With this add-on for Alexander Grau's Ardumower Sunray Software (https://github.com/Ardumower/Sunray) you can add manual remote control to your Alfred (and Ardumower?).

An SBUS receiver (e.g. FrSKy) is used to receive the remote control data and an ESP32 Dev Board is used for post-processing and converting the data to an I²C interface.

There are three operation modes which can be choosen from the RC transmitter:
1. Automatic mode as usual
2. Manual movement control without mow motor
3. Manual movement control with running mow motor
   
If a failsafe occurs or if the RC transmitter is switched off, all motors are stopped and the usual automatic mode switches on again.

1. First of all you need to put the hardware together

2. Now you need to find the channels you like to use and the min, mid and max values of the channels you like to use

3. At the end you are able to set the right settings in the sketch, compile and upload it for productive usage.


The sketch "SBUS_remote_control" does the following:

For better handling two exponential curves can be used to make the throttle and steering smooth to gain a perfect control of your rover.
The exponential curves are exactly the same like the FrSky Taranis X9 Lite S. It took some time you understand how it is generated but finally we were able to figure it out.
A value of "0" corresponds to a linear curve without influence. But this is not recommend because the handling of the rover is not easy and exact.
A negative value will add negative exponential behavior whichs leads to more aggressive reaction of the rover, which is also not recommend.
The best option is a positive value of about 30 for throttle and 30 for steering or similar to earn smooth and exact control.


