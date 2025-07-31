#include <Arduino.h>
#include <SoftwareSerial.h>

SoftwareSerial mySerial(6, 7); // D6 = RX, D7 = TX

// それぞれのコントローラの情報が格納される変数
int left_X, left_Y, right_X, right_Y;
bool SW_left, SW_right, SW_up, SW_down;
bool SW_square, SW_circle, SW_triangle, SW_cross;
bool SW_left1, SW_left2, SW_right1, SW_right2;
// データの格納・分解に使う関数
void DataBreakPut(String PutData);

void setup()
{
  Serial.begin(115200);
  mySerial.begin(9600);
}

void loop()
{
  if (mySerial.available())
  {
    String Px3_Data = mySerial.readStringUntil('\n');
    DataBreakPut(Px3_Data);

    Serial.println(Px3_Data);
  }
  //Serial.println("left_X:  " + String(left_X) + "\tleft_Y:  " + String(left_Y) + "\tright_X:  " + String(right_X) + "\tright_Y:  " + String(right_Y));

  //Serial.print("SW_left:  " + String(SW_left) + "\tSW_right:  " + String(SW_right) + "\tSW_up:  " + String(SW_up) + "\tSW_down:  " + String(SW_down));
  //Serial.print("\tSW_sq:  " + String(SW_square) + "\tSW_ci:  " + String(SW_circle) + "\tSW_tr:  " + String(SW_triangle) + "\tSW_cr:  " + String(SW_cross));
  //Serial.println("\tSW_l1:  " + String(SW_left1) + "\tSW_l2:  " + String(SW_left2) + "\tSW_r1:  " + String(SW_right1) + "\tSW_r2:  " + String(SW_right2));
  delay(100);
}

void DataBreakPut(String PutData)
{
  //正しくデータが来ない場合のプログラム
  if(PutData.length() > 32)PutData = "0,0,0,0,000000000000";

  int Ps3_recoad[4];

  int count = 0;
  for (int i = 0; i < 17; i++)
  {
    if (PutData.charAt(i) == ',')
    {
      Ps3_recoad[count] = i;
      count++;
    }
  }
  // それぞれの変数にPs3のデータを代入
  left_X = PutData.substring(0, Ps3_recoad[0]).toInt();
  left_Y = PutData.substring(Ps3_recoad[0] + 1, Ps3_recoad[1]).toInt();
  right_X = PutData.substring(Ps3_recoad[1] + 1, Ps3_recoad[2]).toInt();
  right_Y = PutData.substring(Ps3_recoad[2] + 1, Ps3_recoad[3]).toInt();

  SW_left = PutData.charAt(Ps3_recoad[3] + 1) - '0';
  SW_right = PutData.charAt(Ps3_recoad[3] + 2) - '0';
  SW_up = PutData.charAt(Ps3_recoad[3] + 3) - '0';
  SW_down = PutData.charAt(Ps3_recoad[3] + 4) - '0';

  SW_square = PutData.charAt(Ps3_recoad[3] + 5) - '0';
  SW_circle = PutData.charAt(Ps3_recoad[3] + 6) - '0';
  SW_triangle = PutData.charAt(Ps3_recoad[3] + 7) - '0';
  SW_cross = PutData.charAt(Ps3_recoad[3] + 8) - '0';

  SW_left1 = PutData.charAt(Ps3_recoad[3] + 9) - '0';
  SW_left2 = PutData.charAt(Ps3_recoad[3] + 10) - '0';
  SW_right1 = PutData.charAt(Ps3_recoad[3] + 11) - '0';
  SW_right2 = PutData.charAt(Ps3_recoad[3] + 12) - '0';
}
