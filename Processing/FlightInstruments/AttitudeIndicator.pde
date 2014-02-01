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
 * This is a copy of Electric Attitude Indicator by Mid-Continent Instrument Co, Model: 4300.
 */

PImage TextureAttitudeIndicatorRollDial;
PImage TextureAttitudeIndicatorFacePanel;
PImage TextureAttitudeIndicatorHorizonBar;
PImage TextureAttitudeIndicatorHorizonFlat;

void loadTextureAttitudeIndicator() {
  TextureAttitudeIndicatorRollDial = loadImage("attitude_indicator_roll_dial.png");
  TextureAttitudeIndicatorFacePanel = loadImage("attitude_indicator_face_panel.png");
  TextureAttitudeIndicatorHorizonBar= loadImage("attitude_indicator_horizon_bar.png");
  TextureAttitudeIndicatorHorizonFlat = loadImage("attitude_indicator_horizon_flat.png");
}

void drawAttitudeIndicator(float rotateZ, float moveX) {
  int cDetail =  10;  // Circle detalization
    
  // draw textured Attitude Indicator's Horizon Flat
  pushMatrix();
    drawAttitudeIndicatorHorizonFlat();
  popMatrix();

  // draw textured Attitude Indicator's Horizon Bar
  pushMatrix();
    drawAttitudeIndicatorHorizonBar(rotateZ, moveX);
  popMatrix();
    
  // draw textured Attitude Indicator's Rotating Roll Dial
  pushMatrix();
    drawAttitudeIndicatorRollDial(80, 330, rotateZ);
  popMatrix();
    
  // draw textured Attitude Indicator's Panel
  pushMatrix();
    drawAttitudeIndicatorFacePanel();
  popMatrix();
}

void drawAttitudeIndicatorRollDial(int cDetail, float r, float rotateDial) {
  beginShape();
    noStroke();
    textureMode(NORMAL);
    rotateZ(rotateDial);
    texture(TextureAttitudeIndicatorRollDial);
    float theta = TWO_PI / cDetail;
    for (int i=0; i<cDetail; i++) {
      float angle = theta * i;
      float x = cos(angle);
      float y = sin(angle);
      vertex(x * r, y * r, (x+1)/2, (y+1)/2);
    }
  endShape();
}

void drawAttitudeIndicatorFacePanel() {
  beginShape();
    noStroke();
    textureMode(IMAGE);
    texture(TextureAttitudeIndicatorFacePanel);
    vertex(-394, -394, 0, 0);
    vertex(394, -394, 788, 0);
    vertex(394, 394, 788, 788);
    vertex(-394, 394, 0, 788);
  endShape();
}

void drawAttitudeIndicatorHorizonFlat() {
  beginShape();
    noStroke();
    textureMode(IMAGE);
    texture(TextureAttitudeIndicatorHorizonFlat);
    vertex(-330, -330, 0, 0);
    vertex(330, -330, 660, 0);
    vertex(330, 330, 660, 660);
    vertex(-330, 330, 0, 660);
  endShape();
}

void drawAttitudeIndicatorHorizonBar(float rotateBar, float moveBar) {
  float newX = map(moveBar, -1, 1, -150, 150);
  
  beginShape();
    noStroke();
    textureMode(IMAGE);  
    rotateZ(rotateBar);
    translate(0, newX);
    texture(TextureAttitudeIndicatorHorizonBar);
    vertex(-264, -164, 0, 0);
    vertex(264, -164, 528, 0);
    vertex(264, 164, 528, 328);
    vertex(-264, 164, 0, 328);
  endShape();
}
