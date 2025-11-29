//-----------------------------------------------------------------------
// コントローラーでアームロボットを動かす
//-----------------------------------------------------------------------
// Copyright (C) Future Robotics Technology Center All Right Reserved
//-----------------------------------------------------------------------
// ライブラリを読み込む
#include <Arduino.h>
#include <RPlib.h>
#include <VarSpeedServo.h>
#include <PS2X_lib.h>

#define ARM1_LONG 148 // Arm1の長さ
#define ARM2_LONG 160 // Arm2の長さ

#define ARM1_MINANGLE 20  // 第一関節の最小角度
#define ARM1_MAXANGLE 110 // 第一関節の最大角度
#define ARM2_MINANGLE 20  // 第二関節の最小角度
#define ARM2_MAXANGLE 150 // 第二関節の最大角度

double Arm1Angle = ARM1_MINANGLE, Arm2Angle = ARM2_MINANGLE; // 第一第二関節の角度を格納する変数
void ArmAngleGet(double X, double Y);                        // サーボモータをどの角度に回せばいいか計算してくれる関数

VarSpeedServo ServoArm2;  // 第一関節のサーボモーター
VarSpeedServo ServoArm1;  // 第二関節のサーボモーター
VarSpeedServo ServoTurn;  // 回転する時のサーボモータ
VarSpeedServo ServoCatch; // キャッチする時のサーボモータ

PS2X ps2x; // コントローラーに名前をつける

double xx = 0, yy = 0; // 行くべき座標を格納する変数

void setup()
{
  pinMode(D3, INPUT_PULLUP);                       // スイッチのピン設定
  ps2x.config_gamepad(13, 11, 10, 12, true, true); // コントローラーの設定

  ServoArm1.attach(S1, 0, 180);  // 第一関節のサーボモータはS1に接続
  ServoArm2.attach(S0, 0, 180);  // 第二関節のサーボモータはS0に接続
  ServoTurn.attach(S2, 0, 180);  // 回転する時のサーボモーターはS2に接続
  ServoCatch.attach(S3, 0, 180); // キャッチする時のサーボモーターはS3に接続
}

void loop()
{
  ps2x.read_gamepad(); // コントローラーから値を読み取る

  int ps2_ly = ps2x.Analog(PSS_LY); // アナログ左スティック上下
  //  int ps2_lx = ps2x.Analog(PSS_LX); // アナログ左スティック左右
  int ps2_ry = ps2x.Analog(PSS_RY); // アナログ右スティック上下
  //  int ps2_rx = ps2x.Analog(PSS_RX); // アナログ右スティック左右

  if (ps2_ly > 0)
    xx++;
  else if (ps2_ly < 0)
    xx--;

  if (ps2_ry > 0)
    yy++;
  else if (ps2_ry < 0)
    yy--;

  ArmAngleGet(xx, yy); // 第一関節第二関節の角度を計算
  ServoArm1.write(Arm1Angle);
  ServoArm2.write(Arm2Angle);

  delay(100); // 100ms待つ
}

void ArmAngleGet(double X, double Y)
{
  if (X == 0 && Y == 0) // もしどちらも0ならば
  {
    Arm2Angle = Arm1Angle; // そのままの値を返す
    Arm2Angle = Arm2Angle; // そのままの値を返す
  }
  else
  {
    // 定数
    double X2_Y2 = pow(X, 2) + pow(Y, 2); // X^2 + Y^2
    double m = ARM1_LONG;                 // 第一関節の長さ
    double m2 = pow(m, 2);                // mの二乗
    double n = ARM2_LONG;                 // 第二関節の長さ
    double n2 = pow(n, 2);                // nの二乗

    // 角度を計算
    double c = acos((m2 + n2 - X2_Y2) / (2 * m * n)); // 第一関節と第二関節の間の角を計算
    c = degrees(c);                                   // 度数法に置換

    double a_up = atan2(Y, X) + acos((X2_Y2 + m2 - n2) / (2 * m * sqrt(X2_Y2))); // 解の1つ目の第一関節角を計算
    a_up = degrees(a_up);                                                        // 度数法に置換

    double b_up = 180 - a_up - c; // 解の1つ目の第二関節角を計算

    double a_down = atan2(Y, X) - acos((X2_Y2 + m2 - n2) / (2 * m * sqrt(X2_Y2))); // 解の2つ目の第一関節角を計算
    a_down = degrees(a_down);                                                      // 度数法に置換

    double b_down = 180 - a_down - c; // 解の2つ目の第二関節角を計算

    // 最適な角度を選出
    double middle_a = (ARM1_MINANGLE + ARM1_MAXANGLE) / 2;                       // 第一関節の回転範囲の真ん中を格納する変数
    Arm1Angle = (abs(a_up - middle_a) < abs(a_down - middle_a)) ? a_up : a_down; // 真ん中により近い角度を選出
    Arm1Angle = constrain(Arm1Angle, ARM1_MINANGLE, ARM1_MAXANGLE);              // 範囲の中に収める

    double middle_b = (ARM2_MINANGLE + ARM2_MAXANGLE) / 2;                       // 第一関節の回転範囲の真ん中を格納する変数
    Arm2Angle = (abs(b_up - middle_b) < abs(b_down - middle_b)) ? b_up : b_down; // 真ん中により近い角度を選出
    Arm2Angle = constrain(Arm2Angle, ARM2_MINANGLE, ARM2_MAXANGLE);              // 範囲の中に収める
  }
}
