/*
 * Copyright (c) 2014 Ovcharenko Alexander <ferrocentos[dot]gmail.com>
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

/*
 * This is a copy of Directional Gyro by Mid-Continent Instrument Co, Model: 3300.
 */

PImage TextureDirectionalGyroRollDial;
PImage TextureDirectionalGyroFacePanel;

void loadTextureDirectionalGyro() {
  TextureDirectionalGyroRollDial = loadImage("directional_gyro_roll_dial.png");
  TextureDirectionalGyroFacePanel = loadImage("directional_gyro_face_panel.png");
}

void drawDirectionalGyro(float rotateZ) {
  int cDetail =  10;  // Circle detalization
  
  // draw textured Directional Gyro's Rotating Roll Dial
  pushMatrix();
    drawDirectionalGyroRollDial(80, 310, rotateZ);
  popMatrix();
  
  // draw textured Directional Gyro's Horizon Flat
  pushMatrix();
    drawDirectionalGyroFacePanel();
  popMatrix();
}

void drawDirectionalGyroFacePanel() {
  beginShape();
    noStroke();
    textureMode(IMAGE);
    texture(TextureDirectionalGyroFacePanel);
    vertex(-394, -394, 0, 0);
    vertex(394, -394, 788, 0);
    vertex(394, 394, 788, 788);
    vertex(-394, 394, 0, 788);
  endShape();
}

void drawDirectionalGyroRollDial(int cDetail, float r, float rotateDial) {
  beginShape();
    noStroke();
    textureMode(NORMAL);
    rotateZ(rotateDial);
    texture(TextureDirectionalGyroRollDial);
    float theta = TWO_PI / cDetail;
    for (int i=0; i<cDetail; i++) {
      float angle = theta * i;
      float x = cos(angle);
      float y = sin(angle);
      vertex(x * r, y * r, (x+1)/2, (y+1)/2);
    }
  endShape();
}
