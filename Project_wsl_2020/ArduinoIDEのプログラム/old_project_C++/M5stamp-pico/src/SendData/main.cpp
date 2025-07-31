#include <Arduino.h>
#include <Ps3Controller.h>

// SelectボタンyあStartボタンがどれだけ押されつづけたかを記録
const int Select_Start_PusshingTime = 20;

// それぞれのコントローラの情報が格納される変数
String left_X, left_Y, right_X, right_Y;
int int_left_X, int_left_Y, int_right_X, int_right_Y;
bool SW_up, SW_down, SW_right, SW_left;
bool SW_triangle, SW_cross, SW_circle, SW_square;
bool SW_left1, SW_left2, SW_right1, SW_right2;
bool SW_select, SW_start;
bool Old_SW[2] = {0, 0};            // 昔のSWのデータをいれる。今回はselectとstartを入れる。
unsigned long Old_time[2] = {0, 0}; // 昔のtimerのデータをいれる。今回はselectとstartのための物

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
  Ps3.begin("4C:75:25:C3:06:D6");

  while (!Ps3.isConnected())
  {
    Serial.println("0000,0000,0000,0000,00000000000000");
    delay(100);
  }
}

void loop()
{
  // Serial.println("left_X:  " + String(left_X) + "\tleft_Y:  " + String(left_Y) + "\tright_X:  " + String(right_X) + "\tright_Y:  " + String(right_Y));
  /*
  Serial.print("SW_left:  " + String(SW_left) + "\tSW_right:  " + String(SW_right) + "\tSW_up:  " + String(SW_up) + "\tSW_down:  " + String(SW_down));
  Serial.print("\tSW_sq:  " + String(SW_square) + "\tSW_ci:  " + String(SW_circle) + "\tSW_tr:  " + String(SW_triangle) + "\tSW_cr:  " + String(SW_cross));
  Serial.println("\tSW_l1:  " + String(SW_left1) + "\tSW_l2:  " + String(SW_left2) + "\tSW_r1:  " + String(SW_right1) + "\tSW_r2:  " + String(SW_right2));
  */
  Serial.println(left_X + "," + left_Y + "," + right_X + "," + right_Y + "," + String(SW_left) + String(SW_right) + String(SW_up) + String(SW_down) + String(SW_square) + String(SW_circle) + String(SW_triangle) + String(SW_cross) + String(SW_left1) + String(SW_left2) + String(SW_right1) + String(SW_right2) + String(SW_select) + String(SW_start));
  delay(100);
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
    Serial.println("0000,0000,0000,0000,00000000000000");
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

  if (Ps3.data.button.select)
  {
    if (Old_SW[0] == 0)
    {
      Old_time[0] = millis(); // 押し始めた瞬間だけ更新
    }

    if ((millis() - Old_time[0]) > Select_Start_PusshingTime)
    {
      SW_select = 1; // 押され続けていて、一定時間超えたら1
    }
    else
    {
      SW_select = 0;
    }

    Old_SW[0] = 1;
  }
  else
  {
    SW_select = 0;
    Old_SW[0] = 0;
  }
  if (Ps3.data.button.start)
  {
    if (Old_SW[1] == 0)
    {
      Old_time[1] = millis(); // 押し始めた瞬間だけ更新
    }

    if ((millis() - Old_time[1]) > Select_Start_PusshingTime)
    {
      SW_start = 1; // 押され続けていて、一定時間超えたら1
    }
    else
    {
      SW_start = 0;
    }

    Old_SW[1] = 1;
  }
  else
  {
    SW_start = 0;
    Old_SW[1] = 0;
  }
}