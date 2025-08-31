#include <Arduino.h>
#include "M5STAMPps3.h"

void setup()
{
  Serial.begin(9600);
  M5STAMPps3begin();
}

void loop()
{
  M5STAMPps3get();
  Serial.print(" Stickdata: " + String(Left_X) + " , " + String(Left_Y) + " , " + String(Right_X) + " , " + String(Right_Y));
  Serial.println(" SWdata: " + String(SW_up) + " , " + String(SW_down) + " , " + String(SW_right) + " , " + String(SW_left) + " , " + String(SW_triangle) + " , " + String(SW_cross) + " , " + String(SW_circle) + " , " + String(SW_square) + " , " + String(SW_left1) + " , " + String(SW_left2) + " , " + String(SW_right1) + " , " + String(SW_right2));
  delay(100);
}