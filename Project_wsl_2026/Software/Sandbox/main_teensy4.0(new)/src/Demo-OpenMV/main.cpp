#include <Arduino.h>

#include "SSD1306.h"
#include "OpenMV.h"

void setup()
{
  Serial.begin(9600); // デバッグ用

  SSD1306begin();
  SSD1306clear();

  OpenMVbegin();
}

void loop()
{
  OpenMVget(); // data取得

  SSD1306clear(); // 削除

  SSD1306write(1, 0, 0, String("C_edge : ") + String(Soccer_field_max_deg), false);

  SSD1306write(1, 0, 10, String(Yellow_goal_exist), false);
  SSD1306write(1, 0, 20, String("Y_edge : ") + String(Yellow_goal_deg), false);
  SSD1306write(1, 0, 30, String("Y_dist : ") + String(Yellow_goal_distance), false);

  SSD1306write(1, 0, 40, String(Blue_goal_exist), false);
  SSD1306write(1, 0, 50, String("B_edge : ") + String(Blue_goal_deg), false);
  SSD1306write(1, 0, 60, String("B_dist : ") + String(Blue_goal_distance), false);

  SSD1306show(); // 表示

  delay(100);
}
