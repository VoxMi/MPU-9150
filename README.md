MPU-9150
============
This repository contains the source code of library for use with popular IMU sensors from **InvenSense®** as *MPU-6500, MPU-6050, MPU-9150 and MPU-9250*. Library based on **"Embedded MotionDriver v5.1"** and is designed for run on **Atmel AVR 8-bit Microcontrollers** with sufficient memory (FLASH memory size must be at least **32Kbytes** and SRAM no less than **4Kbytes**).

This code compiled by **WinAVR (20121007)** and tested on *ATMega2560*.

Processing demo file
--------------------
At the `Processing/MPUAirplane` you can find a simple **"Processing 2.1"** sketch for visualizing the work with IMU sensors. Additional dependencies for **Processing**: *ToxicLibs* and *OBJLoader* libraries.

At the `Processing/FlightInstruments` you can find sketch with model of attitude indicator and directional gyro that simulates the earth’s horizon and сourse deviation indicator. This is a copies of the real flight instruments called an **"Electric Attitude Indicator (Model: 4300)"** and **"Directional Gyro Indicator (Model: 3300)"** by *Mid-Continent Instrument Co*.

![Processing Demo](http://i.imgur.com/lVR7FXa.jpg "Processing Demo")

NOTE
----
This source code does not contain any filters prevent drift from y-axis (YAW).
