#include <Arduino.h>
#include "DSR1202.h"
#include "LINEXIAO.h"
#include "IRXIAO.h"
#include "SSD1306.h"

/*モータ制御について*/
// rfモータのは1chに接続
// lfモータのは2chに接続
// rbモータのは3chに接続
// lbモータのは4chに接続
void Motor4Control(double speed_move, double edge_move); // ロボットを指定した角度に移動させる関数
void Motor4Stop();                                       // ロボットを停止させる関数

void setup()
{
  LINEXIAObegin();
  IRXIAObegin();
  DSR1202begin();
  SSD1306begin();

  Serial.begin(9600);
}

void loop()
{
  LINEXIAOget();
  IRXIAOget();

  if (LINEexist)
  {
    Motor4Control(50, LINEedge + 180);
    SSD1306write(1, 0, 0, "LINE" + String(LINEedge), false);
  }
  else
  {
    if (IRexist)
    {
      Motor4Control(50, IRedge);
      SSD1306write(1, 0, 0, "IR" + String(IRedge), false);
    }
    else
    {
      Motor4Stop();
      SSD1306write(1, 0, 0, "Nothing", false);
    }
  }

  SSD1306show();
  delay(50);
  SSD1306clear();
}

/*　ロボットを指定した角度に移動させる関数　*/
void Motor4Control(double speed_move, double edge_move)
{
  double ControlmotorPower[4] = {0.0, 0.0, 0.0, 0.0};                // モータの出力値を格納する配列 {rf, lf, rb, lb}
  ControlmotorPower[0] = -sin(radians(edge_move + 45)) * speed_move; // rfモータの出力値
  ControlmotorPower[1] = cos(radians(edge_move + 45)) * speed_move;  // lfモータ2の出力値
  ControlmotorPower[2] = -cos(radians(edge_move + 45)) * speed_move; // rbモータ3の出力値
  ControlmotorPower[3] = sin(radians(edge_move + 45)) * speed_move;  // lbモータ4の出力値

  DSR1202move(ControlmotorPower[0],
              ControlmotorPower[1],
              ControlmotorPower[2],
              ControlmotorPower[3]); // モータを制御する
}

void Motor4Stop()
{
  DSR1202move(0, 0, 0, 0); // モータを停止する
}