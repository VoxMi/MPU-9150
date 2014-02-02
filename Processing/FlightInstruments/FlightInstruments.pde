/*
 * I2Cdev device library code is placed under the MIT license
 * Copyright (c) 2012 Jeff Rowberg
 *
 * Modified by Ovcharenko Alexander <ferrocentos[dot]gmail.com>
 * Added model of attitude indicator that simulates the earth’s horizon.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:

 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

import processing.opengl.*;
import processing.serial.*;
import toxi.geom.*;
import toxi.processing.*;

Serial port;                         // The serial port
char[] teapotPacket = new char[23];  // InvenSense Teapot packet
int serialCount = 0;                 // current packet byte position
int interval = 0;

char[] temp = new char[4];
float[] q = new float[4];
Quaternion quat = new Quaternion(1, 0, 0, 0);

float[] gravity = new float[3];
float[] euler = new float[3];
float[] ypr = new float[3];

void setup() {
  size(1200, 600, OPENGL);

  loadTextureAttitudeIndicator();
  loadTextureDirectionalGyro();
    
  // setup lights and antialiasing
  lights();
  smooth(4);
    
  // display serial port list for debugging/clarity
  println(Serial.list());

  // get the first available port (use EITHER this OR the specific port code below)
  String portName = Serial.list()[0];
    
  // get a specific serial port (use EITHER this OR the first-available code above)
  //String portName = "COM6";
    
  // open the serial port
  port = new Serial(this, portName, 115200);
}

void draw() {
  background(0);
    
  // toxiclibs direct angle/axis rotation from quaternion (NO gimbal lock!)
  // (axis order [1, 3, 2] and inversion [-1, +1, +1] is a consequence of
  // different coordinate system orientation assumptions between Processing
  // and InvenSense DMP)
  float[] axis = quat.toAxisAngle();
    
  // draw Attitude Indicator (copy of Electric Attitude Indicator by Mid-Continent Instrument Co, Model: 4300-411)
  pushMatrix();
    translate(width/4, height/2);
    scale(0.75);
    drawAttitudeIndicator(-ypr[2], -ypr[1]);
  popMatrix();
  
  // draw Directional Gyro
  pushMatrix();
    translate(3*width/4, height/2);
    scale(0.75);
    drawDirectionalGyro(-ypr[0]);
  popMatrix();
}

void serialEvent(Serial port) {
  while (port.available() > 0) {
    int ch = port.read();    
    if (serialCount > 0 || ch == '$') {
      teapotPacket[serialCount++] = (char)ch;
      if (serialCount == 23) {
        serialCount = 0; // restart packet byte position
                    
        // get quaternion from data packet
        q[0] = ((teapotPacket[2] << 24) | (teapotPacket[3] << 16)| (teapotPacket[4] << 8) | teapotPacket[5]) / 1073741824.0f;
        q[1] = ((teapotPacket[6] << 24) | (teapotPacket[7] << 16)| (teapotPacket[8] << 8) | teapotPacket[9]) / 1073741824.0f;
        q[2] = ((teapotPacket[10] << 24) | (teapotPacket[11] << 16)| (teapotPacket[12] << 8) | teapotPacket[13]) / 1073741824.0f;
        q[3] = ((teapotPacket[14] << 24) | (teapotPacket[15] << 16)| (teapotPacket[16] << 8) | teapotPacket[17]) / 1073741824.0f;
                    
        // get temperature
        temp[0] = teapotPacket[18];
        temp[1] = teapotPacket[19];
        temp[2] = teapotPacket[20];
        temp[3] = teapotPacket[21];
                    
        for (int i = 0; i < 4; i++) if (q[i] >= 2) q[i] = -4 + q[i];
          // set our toxilibs quaternion to new data
          quat.set(q[0], q[1], q[2], q[3]);
                   
          // below calculations unnecessary for orientation only using toxilibs
                    
          // calculate gravity vector
          gravity[0] = 2 * (q[1]*q[3] - q[0]*q[2]);
          gravity[1] = 2 * (q[0]*q[1] + q[2]*q[3]);
          gravity[2] = q[0]*q[0] - q[1]*q[1] - q[2]*q[2] + q[3]*q[3];

          // calculate Euler angles
          /*euler[0] = atan2(2*q[1]*q[2] - 2*q[0]*q[3], 2*q[0]*q[0] + 2*q[1]*q[1] - 1);
          euler[1] = -asin(2*q[1]*q[3] + 2*q[0]*q[2]);
          euler[2] = atan2(2*q[2]*q[3] - 2*q[0]*q[1], 2*q[0]*q[0] + 2*q[3]*q[3] - 1);*/

          // calculate yaw/pitch/roll angles
          ypr[0] = atan2(2*q[1]*q[2] - 2*q[0]*q[3], 2*q[0]*q[0] + 2*q[1]*q[1] - 1);
          ypr[1] = atan(gravity[0] / sqrt(gravity[1]*gravity[1] + gravity[2]*gravity[2]));
          ypr[2] = atan(gravity[1] / sqrt(gravity[0]*gravity[0] + gravity[2]*gravity[2]));                   
        }
      }
  }
}
