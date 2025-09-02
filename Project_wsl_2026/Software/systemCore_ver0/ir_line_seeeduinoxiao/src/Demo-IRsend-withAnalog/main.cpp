#include <Arduino.h>
#include "MySettings.h"

const int Update_Ms = 50; // IRボールの角度・距離をどのくらいの周期で計算するか

// 円形IRについての設定　<マルチプレクサ経由で読み取る>
const int IRs = 8;                                   // IRを何個使うか指定
const int IRpinNum[IRs] = {3, 10, 9, 8, 7, 6, 5, 4}; // IRのピンを(前から時計回りに)指定
const int IRmaxVal = 1000;                           // IRセンサーでボールがあると判断する最大に値を指定
const double R = 50.0;                               // 円形に設置したIRの半径を指定
const double IR_pqAdjust = 0.002;                    // IRボールの座標を求めるための係数を指定
const double IRdistAdjust = 0.1;                     // IRボールまでの距離をmmに直すための係数を指定

int IRjudgeTotal;                       //  IRボールがあるのかどうか判定する変数
double IRxNum[IRs], IRyNum[IRs];        // それぞれのIRの座標を格納するための変数
long double IRsumX = 0.0, IRsumY = 0.0; // IRボール方向を出すための座標を格納する変数
double IRedge;                          // IRボールの角度を格納するための変数

long double IRsumP = 0.0, IRsumQ = 0.0; // IRボールの座標を格納するための変数
double IRdist;                          // IRボールはどれくらい離れているかの距離を格納するための変数

void setup()
{
  // シリアルの初期設定
  Serial.begin(9600);                   // PCとの通信開始。ボートレートは9600とする。
  Serial1.begin(IRSEND_SERIALSPEED); // Teensyとの通信開始。ボートレートは9600とする。

  // IRセンサーのピンモード等を設定
  for (int i = 0; i < IRs; i++)
  {
    pinMode(IRpinNum[i], INPUT);                 // IRを使うピンをINPUTに設定
    IRxNum[i] = sin(radians(i * 360 / IRs)) * R; // IRセンサーのX座標を格納
    IRyNum[i] = cos(radians(i * 360 / IRs)) * R; // IRセンサーのY座標を格納
  }
}

void loop()
{
  // IRボールの角度・距離・座標を計算する
  IRsumX = 0.0;     // IRボールのX座標を初期化
  IRsumY = 0.0;     // IRボールのY座標を初期化
  IRsumP = 0.0;     // IRボールのP座標を初期化
  IRsumQ = 0.0;     // IRボールのQ座標を初期化
  IRjudgeTotal = 0; // IRボールがあるかどうかの総和を初期化
  for (int i = 0; i < IRs; i++)
  {
    int IRval = analogRead(IRpinNum[i]); // "i"ピンのIRセンサーの値を格納

    bool IRseeJudge = (IRval < IRmaxVal); // IRボールがあるかどうか判定する
    IRjudgeTotal += IRseeJudge;           // それぞれのIRセンサーの結果を足し算していく

    IRsumX += IRxNum[i] * (1023 - IRval) * IRseeJudge; // それぞれのIRセンサーの値からIRボールのX座標をその総和で特定
    IRsumY += IRyNum[i] * (1023 - IRval) * IRseeJudge; // それぞれのIRセンサーの値からIRボールのY座標をその総和で特定

    IRsumP += -IRxNum[i] * IRval * IRseeJudge * IR_pqAdjust; // それぞれのIRセンサーの値からIRボールのp座標をその総和で特定
    IRsumQ += -IRyNum[i] * IRval * IRseeJudge * IR_pqAdjust; // それぞれのIRセンサーの値からIRボールのq座標をその総和で特定
  }

  if (IRjudgeTotal == 0)
  {
    IRedge = -1; // すべてのIRセンサーが反応していなかった場合、ありえない値の-1をIRedgeとする
    IRdist = -1; // すべてのIRセンサーが反応していなかった場合、ありえない値の-1をIRdistとする
  }
  else
  {
    // IRボールの角度を座標から計算
    IRedge = 90.0 - degrees(atan2(IRsumY, IRsumX)); // アークタンジェントを使用
    if (IRedge < 0)
      IRedge += 360.0; // 0〜359の範囲に収める処理
    if (IRedge >= 360.0)
      IRedge -= 360.0; // 0〜359の範囲に収める処理

    // IRボールまでの距離を座標から計算
    IRdist = sqrt(IRsumP * IRsumP + IRsumQ * IRsumQ) * IRdistAdjust; // ピタゴラスの定理を使用
  }

  // シリアル通信で送信する
  String IRedgeSendData = String(int(IRedge * 10));
  if (IRedgeSendData.length() > 4)
    IRedgeSendData = "9999";
  while (IRedgeSendData.length() < 4)
  {
    IRedgeSendData = "0" + IRedgeSendData;
  } // 値を整形する

  String IRdistSendData = String(int(IRdist * 10));
  if (IRdistSendData.length() > 4)
    IRdistSendData = "9999";
  while (IRdistSendData.length() < 4)
  {
    IRdistSendData = "0" + IRdistSendData;
  } // 値を整形する

  String IRsumPSendData = String(int(IRsumP));
  if (IRsumPSendData.length() > 4)
    IRsumPSendData = "9999";
  while (IRsumPSendData.length() < 4)
  {
    IRsumPSendData = "0" + IRsumPSendData;
  } // 値を整形する

  String IRsumQSendData = String(int(IRsumQ));
  if (IRsumQSendData.length() > 4)
    IRsumQSendData = "9999";
  while (IRsumQSendData.length() < 4)
  {
    IRsumQSendData = "0" + IRsumQSendData;
  } // 値を整形する

  Serial.println("a" + IRedgeSendData + "b" + IRdistSendData + "c" + IRsumPSendData + "d" + IRsumQSendData); // PCに送信
  Serial1.println("a" + IRedgeSendData);
  Serial1.println("b" + IRdistSendData);
  Serial1.println("c" + IRsumPSendData);
  Serial1.println("d" + IRsumQSendData); // Teensyに送信

  // 待つ
  delay(Update_Ms); // Update_Ms分待つ
}