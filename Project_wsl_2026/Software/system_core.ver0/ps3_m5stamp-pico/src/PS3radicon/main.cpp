#include <Arduino.h>
#include <Ps3Controller.h>

// それぞれのコントローラの情報が格納される変数
String left_X, left_Y, right_X, right_Y;
int int_left_X, int_left_Y, int_right_X, int_right_Y;
bool SW_up, SW_down, SW_right, SW_left;
bool SW_triangle, SW_cross, SW_circle, SW_square;
bool SW_left1, SW_left2, SW_right1, SW_right2;

// PS3が接続されると呼ばれる関数
void Connect_success();
// PS3が切断されると呼ばれる関数
void Connect_false();
// PS3のどれが押されると呼ばれる関数
void SurveyData();

void setup()
{
  Serial.begin(9600);
  Ps3.attach(SurveyData);
  Ps3.attachOnConnect(Connect_success);  // 接続されたときに呼び出す関数
  Ps3.attachOnDisconnect(Connect_false); // 切断されたときに呼び出す関数
  Ps3.begin("E8:6B:EA:31:0A:3A");

  while (!Ps3.isConnected())
  {
    Serial.println("a0000");
    delay(100);
  }
}

void loop()
{
  Serial.println("a" + String(left_X));
  Serial.println("b" + String(left_Y));
  Serial.println("c" + String(right_X));
  Serial.println("d" + String(right_Y));
  Serial.println("e" + String(SW_left) + String(SW_right) + String(SW_up) + String(SW_down));
  Serial.println("f" + String(SW_square) + String(SW_circle) + String(SW_triangle) + String(SW_cross));
  Serial.println("g" + String(SW_left1) + String(SW_left2) + String(SW_right1) + String(SW_right2));
  delay(50);
}

// それぞれの関数のプログラム

void Connect_success()
{
  Ps3.setRumble(1, 1000);
}

void Connect_false()
{
  while (!Ps3.isConnected())
  {
    Serial.println("a0000");
    delay(100);
  }
}

void SurveyData()
{
  int_left_X = Ps3.data.analog.stick.lx;
  left_X = abs(int_left_X);
  while (left_X.length() < 3)
  {
    left_X = "0" + left_X;
  }
  if (int_left_X < 0)
  {
    left_X = "-" + left_X;
  }
  else
  {
    left_X = "0" + left_X;
  }

  int_left_Y = Ps3.data.analog.stick.ly;
  left_Y = abs(int_left_Y);
  while (left_Y.length() < 3)
  {
    left_Y = "0" + left_Y;
  }
  if (int_left_Y < 0)
  {
    left_Y = "-" + left_Y;
  }
  else
  {
    left_Y = "0" + left_Y;
  }

  int_right_X = Ps3.data.analog.stick.rx;
  right_X = abs(int_right_X);
  while (right_X.length() < 3)
  {
    right_X = "0" + right_X;
  }
  if (int_right_X < 0)
  {
    right_X = "-" + right_X;
  }
  else
  {
    right_X = "0" + right_X;
  }

  int_right_Y = Ps3.data.analog.stick.ry;
  right_Y = abs(int_right_Y);
  while (right_Y.length() < 3)
  {
    right_Y = "0" + right_Y;
  }
  if (int_right_Y < 0)
  {
    right_Y = "-" + right_Y;
  }
  else
  {
    right_Y = "0" + right_Y;
  }

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