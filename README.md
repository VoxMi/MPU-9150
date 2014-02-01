MPU-9150
============
This repository contains the source code of library for use with popular IMU sensors from **InvenSense®** as *MPU-6500, MPU-6050, MPU-9150 and MPU-9250*. Library based on **"Embedded MotionDriver v5.1"** and is designed for run on **Atmel AVR 8-bit Microcontrollers** with sufficient memory (FLASH memory size must be at least **32Kbytes** and SRAM no less than **4Kbytes**).

This code compiled by **WinAVR (20121007)** and tested on *ATMega2560*.

Processing demo file
--------------------
At the `Processing/MPUAirplane` you can find a simple **"Processing 2.1"** sketch for visualizing the work with IMU sensors. Additional dependencies for **Processing**: *ToxicLibs* and *OBJLoader* libraries.

At the `Processing/FlightInstruments` you can find sketch with model of attitude indicator that simulates the earth’s horizon. This is a copy of the real flight instrument called an **"Electric Attitude Indicator"** by *Mid-Continent Instrument Co*, **Model: 4300**.
