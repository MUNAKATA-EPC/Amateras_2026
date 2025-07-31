#include <Arduino.h>
#include <Ps3Controller.h>

// それぞれのコントローラの情報が格納される変数
int left_X, left_Y, right_X, right_Y;
int SW_up, SW_down, SW_right, SW_left;
int SW_triangle, SW_cross, SW_circle, SW_square;
int SW_left1, SW_left2, SW_right1, SW_right2;

// PS3が接続されると呼ばれる関数
void onConnect();
// PS3のどれが押されると呼ばれる関数
void SurveyData();

void setup()
{
  Serial.begin(115200);
  Ps3.attach(SurveyData);
  Ps3.attachOnConnect(onConnect);
  Ps3.begin("4C:75:25:C3:06:D6");
}

void loop()
{
  //Serial.println("left_X:  " + String(left_X) + "\tleft_Y:  " + String(left_Y) + "\tright_X:  " + String(right_X) + "\tright_Y:  " + String(right_Y));
  Serial.print("\tSW_up:  " + String(SW_up) + "\tSW_down:  " + String(SW_down) + "\tSW_left:  " + String(SW_left) + "\tSW_right:  " + String(SW_right));
  Serial.print("\tSW_tr:  " + String(SW_triangle) + "\tSW_cr:  " + String(SW_cross) + "\tSW_sq:  " + String(SW_square) + "\tSW_ci:  " + String(SW_circle));
  Serial.println("\tSW_l1:  " + String(SW_left1) + "\tSW_l2:  " + String(SW_left2) + "\tSW_r1:  " + String(SW_right1) + "\tSW_r2:  " + String(SW_right2));
  delay(100);
}

// それぞれの関数のプログラム

void onConnect()
{
  Ps3.setRumble(1, 1000);
}

void SurveyData()
{
  left_X = Ps3.data.analog.stick.lx;
  left_Y = Ps3.data.analog.stick.ly;
  right_X = Ps3.data.analog.stick.rx;
  right_Y = Ps3.data.analog.stick.ry;

  SW_up = Ps3.data.button.up;
  SW_down = Ps3.data.button.down;
  SW_left = Ps3.data.button.left;
  SW_right = Ps3.data.button.right;

  SW_triangle = Ps3.data.button.triangle;
  SW_cross = Ps3.data.button.cross;
  SW_square = Ps3.data.button.square;
  SW_circle = Ps3.data.button.circle;

  SW_left1 = Ps3.data.button.l1;
  SW_left2 = Ps3.data.button.l2;
  SW_right1 = Ps3.data.button.r1;
  SW_right2 = Ps3.data.button.r2;
}