#ifndef EYES_H
#define EYES_H

#include <Adafruit_SSD1306.h>
#include <Arduino.h>
#include <Adafruit_GFX.h>

void initEyes(Adafruit_SSD1306& display);
void draw_eyes(int left_eye_x, int left_eye_y, int right_eye_x, int right_eye_y);
void center_eyes();
void blink(int speed);
void lookLeft();
void lookRight();
void saccade();

void cry(float health);
void sad(float health);
void normal(float health);
void happy(float health);
void super_happy(float health);

#endif