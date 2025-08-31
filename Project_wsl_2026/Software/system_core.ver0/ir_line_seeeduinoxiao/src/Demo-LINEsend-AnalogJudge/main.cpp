/*フォトトランジスタを使う場合のプログラム*/
#include <Arduino.h>
#include "MyLib-Multi16ch.h"
#include "MySettings.h"

const int Update_Ms = 50; // LINEの角度などをどのくらいの周期で計算するか

// 円形ラインについての設定　<マルチプレクサ経由で読み取る>
const int LINEs = 16;                                                                 // LINE素子を何個使うか指定
const int LINEpinNum[LINEs] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15}; // LINE素子のピンを(前から時計回りに)指定
const int LINEjudgeVal = 350;                                                         // LINE素子の値が何以下だとラインがあると認識するか
const double R = 50.0;                                                                // 円形に設置したLINE基盤の半径を指定

int LINEjudgeTotal;                         // LINEがあるのかどうか判定する変数
double LINExNum[LINEs], LINEyNum[LINEs];    // それぞれのLINE素子の座標を格納するための変数
long double LINEsumX = 0.0, LINEsumY = 0.0; // LINEの座標の総和を格納するための変数
int LINEedge;                            // LINEの角度を格納するための変数

// サイドラインについての設定　<アナログで読み取る>
const int SIDELINE_RIGHTpinNum = 0; // 右サイドのラインのピンを指定
const int SIDELINE_LEFTpinNum = 0;  // 左サイドのラインのピンを指定
const int SIDELINE_DOWNpinNum = 0;  // 後サイドのラインのピンを指定
const int SIDELINEjudgeVal = 600;   // SIDELINE素子の値が何以下だとラインがあると認識するか

bool SIDELINE_RIGHT, SIDELINE_LEFT, SIDELINE_DOWN; // 右・左・後のサイドラインの状況を格納する変数

void setup()
{
  // シリアルの初期設定
  Serial.begin(9600);                  // PCとの通信開始。ボートレートは9600とする。
  Serial1.begin(LINESEND_SERIALSPEED); // Teensyとの通信開始。ボートレートは9600とする。

  // マルチプレクサの初期設定
  MULTI16begin();

  // LINE素子のピンモード等を設定
  for (int i = 0; i < LINEs; i++)
  {
    LINExNum[i] = sin(radians(i * 360 / LINEs)) * R; // IRセンサーのX座標を格納
    LINEyNum[i] = cos(radians(i * 360 / LINEs)) * R; // IRセンサーのY座標を格納
  }
}

void loop()
{
  // LINEの座標を計算
  LINEsumX = 0.0;     // IRボールのX座標を初期化
  LINEsumY = 0.0;     // IRボールのY座標を初期化
  LINEjudgeTotal = 0; // IRボールがあるかどうかの総和を初期化
  for (int i = 0; i < LINEs; i++)
  {
    int LINEval = MULTI16read(LINEpinNum[i]);    // "i"ピンのLINE素子の値を読み取る
    bool LINEcondition = LINEval > LINEjudgeVal; // "i"ピンのLINE素子に反応があるかどうか格納する変数

    LINEjudgeTotal += LINEcondition; // それぞれのLINE素子の結果を足し算していく

    LINEsumX += LINExNum[i] * LINEval; // 足し合わせていく
    LINEsumY += LINEyNum[i] * LINEval; // 足し合わせていく
  }

  // LINEの角度を計算
  if (LINEjudgeTotal == 0)
  {
    LINEedge = -1; // すべてのLINE素子が反応していなかった場合、ありえない値の-1をLINEedgeとする
  }
  else
  {
    // LINEの角度を座標から計算
    LINEedge = int(90 - degrees(atan2(LINEsumY, LINEsumX))); // アークタンジェントを使用
    if (LINEedge < 0)
      LINEedge += 360; // 0〜359の範囲に収める処理
    if (LINEedge >= 360)
      LINEedge -= 360; // 0〜359の範囲に収める処理
  }

  // サイドラインを読み取る
  SIDELINE_RIGHT = (analogRead(SIDELINE_RIGHTpinNum) > SIDELINEjudgeVal);
  SIDELINE_LEFT = (analogRead(SIDELINE_LEFTpinNum) > SIDELINEjudgeVal);
  SIDELINE_DOWN = (analogRead(SIDELINE_DOWNpinNum) > SIDELINEjudgeVal);

  // シリアル通信で送信する
  String LINEedgeSendData;
  if (LINEedge == -1)
  {
    LINEedgeSendData = "---";
  }
  else
  {
    LINEedgeSendData = String(LINEedge);
    while (LINEedgeSendData.length() < 3)
    {
      LINEedgeSendData = "0" + LINEedgeSendData;
    } // 値を整形する
  }
  String SIDELINESendData = String(SIDELINE_RIGHT) + String(SIDELINE_LEFT) + String(SIDELINE_DOWN);

  Serial.println(LINEedgeSendData + SIDELINESendData);
  Serial1.println(LINEedgeSendData + SIDELINESendData);

  // 待つ
  delay(Update_Ms); // Update_Ms分待つ
}