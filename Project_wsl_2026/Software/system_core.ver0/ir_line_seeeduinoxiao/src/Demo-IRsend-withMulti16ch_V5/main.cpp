#include <Arduino.h>
#include "MyLib-Multi16ch.h"
#include "MySettings.h"

const int Update_Ms = 50; // IRボールの角度・距離をどのくらいの周期で計算するか

const int IRs = 16;                                                               // IRを何個使うか指定
const int AroundIRs = 4;                                                          // 角度測定用に最小のIRから±何個のIRを使うか
const int IRpinNum[IRs] = {0, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1}; // IRのピン番号（前から反時計回り）
const int IRmaxVal = 1000;                                                        // IRセンサーでボールがあると判断する最大値
const double R = 10;                                                              // 円形に設置したIRの半径
const double IRgain[IRs] = {                                                      // 個体差補正係数（1.0で統一）
    1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
    1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0};
const double IRmingain = 1.0;  // 最小IRセンサーの値を強調する補正
const double IRdistgain = 0.5; // 距離補正係数（未使用）

// 変数
int IRval[IRs];                    // 各IRセンサー値
int IRedge = -1;                   // ボールの角度
int IRdist = -1;                   // ボールの距離
int IRminindex = 0;                // 最小値のIRセンサーのインデックス
double IRxPosi[IRs], IRyPosi[IRs]; // センサー位置（円周上）
double IRsumx = 0, IRsumy = 0;     // x・y方向の合算値
// double IRfrontx = 0, IRfronty = 0; // 前を正確に出すためのx・y方向の合算値
String IRedgeSendData = "---";  // 送信用文字列（角度）
String IRdistSendData = "----"; // 送信用文字列（距離）

void setup()
{
  Serial.begin(9600);                // PC用シリアル
  Serial1.begin(IRSEND_SERIALSPEED); // Teensy用シリアル

  MULTI16begin(); // マルチプレクサ初期化

  // センサー位置を円周上に配置
  double sensorAngle = 360.0 / IRs;
  for (int i = 0; i < IRs; i++)
  {
    IRxPosi[i] = cos(radians(i * sensorAngle)) * R;
    IRyPosi[i] = sin(radians(i * sensorAngle)) * R;
  }
}

void loop()
{
  IRminindex = 0;

  // センサー値読み取りと最小値探索
  for (int i = 0; i < IRs; i++)
  {
    IRval[i] = MULTI16read(IRpinNum[i]) * IRgain[i];

    if (IRval[i] < IRval[IRminindex])
      IRminindex = i;
  }

  // IRボールの座標・距離計算
  IRsumx = 0;
  IRsumy = 0;
  for (int i = -AroundIRs; i <= AroundIRs; i++)
  {
    int IRreadindex = (i + IRminindex + IRs) % IRs;

    int IRsumval = (IRreadindex == IRminindex) ? (1023 - IRval[IRreadindex]) * IRmingain : (1023 - IRval[IRreadindex]);

    IRsumx += IRsumval * IRxPosi[IRreadindex];
    IRsumy += IRsumval * IRyPosi[IRreadindex];
  }

  // 結果判定
  if (IRval[IRminindex] > IRmaxVal)
  {
    IRedge = -1;
    IRdist = -1;
    IRedgeSendData = "---";
    IRdistSendData = "----";
  }
  else
  {
    // 角度算出
    IRedge = int(degrees(atan2(IRsumy, IRsumx)) + 360) % 360;
    IRedge = (IRedge + 360) % 360;
    IRedgeSendData = String(IRedge);
    while (IRedgeSendData.length() < 3)
      IRedgeSendData = "0" + IRedgeSendData;

    // 距離算出（平均値）
    // IRdist = (IRval[IRminindex] + IRval[(IRminindex + 1 + IRs) % IRs] + IRval[(IRminindex - 1 + IRs) % IRs]) / 3;
    IRdist = int(sqrt(pow(IRsumx, 2) + pow(IRsumy, 2)) / 100);
    IRdist = map(IRdist, 0, 500, 500, 0);
    IRdistSendData = String(IRdist);
    if (IRdistSendData.length() > 4)
      IRdistSendData = "9999";
    while (IRdistSendData.length() < 4)
      IRdistSendData = "0" + IRdistSendData;
  }

  // デバッグ表示 & 送信
  Serial.println("IRedge " + IRedgeSendData +
                 " IRdist " + IRdistSendData +
                 " minPin " + String(IRpinNum[IRminindex]));

  String SendData = IRedgeSendData + IRdistSendData + "a"; // 最後にaを付けて送信 0000000 -> 7文字＋a -> 8文字
  Serial1.write(SendData.c_str(), SendData.length());

  delay(Update_Ms);
}
