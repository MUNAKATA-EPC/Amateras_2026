#include <Arduino.h>

const int Update_Ms = 100; // IRボールの角度・距離をどのくらいの周期で計算するか

const int IRs = 8;                                  // IRを何個使うか指定
const int IRpinNum[IRs] = {1, 8, 7, 6, 5, 4, 3, 2}; // IRのピンを(前から時計回りに)指定
const int IRmaxVal = 1000;                          // IRセンサーでボールがあると判断する最大に値を指定
const double R = 50.0;                              // 円形に設置したIRの半径を指定
const double IRdistAdjust = 100;                    // IRボールまでの距離をmmに直すための係数を指定

int IRjudgeTotal;                       //  IRボールがあるのかどうか判定する変数
double IRxNum[IRs], IRyNum[IRs];        // それぞれのIRの座標を格納するための変数
long double IRsumX = 0.0, IRsumY = 0.0; // IRボールの座標を格納するための変数
double IRedge;                          // IRボールの角度を格納するための変数
double IRdist;                          // IRボールはどれくらい離れているかの距離を格納するための変数

void setup()
{
    // シリアルの初期設定
    Serial.begin(9600); // PCとの通信開始。ボートレートは9600とする。

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
    // IRボールの座標を計算
    IRsumX = 0.0;     // IRボールのX座標を初期化
    IRsumY = 0.0;     // IRボールのY座標を初期化
    IRjudgeTotal = 0; // IRボールがあるかどうかの総和を初期化
    for (int i = 0; i < IRs; i++)
    {
        int IRval = analogRead(IRpinNum[i]); // "i"ピンのIRセンサーの値を格納

        bool IRseeJudge = (IRval < IRmaxVal); // IRボールがあるかどうか判定する
        IRjudgeTotal += IRseeJudge;           // それぞれのIRセンサーの結果を足し算していく

        IRsumX += IRxNum[i] * (1023 - IRval) * IRseeJudge; // それぞれのIRセンサーの値からIRボールのX座標をその総和で特定
        IRsumY += IRyNum[i] * (1023 - IRval) * IRseeJudge; // それぞれのIRセンサーの値からIRボールのY座標をその総和で特定
    }

    // IRボールの角度・距離を計算
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
        IRdist = IRdistAdjust * 10000 / sqrt(pow(IRsumX, 2) + pow(IRsumY, 2)); // 三平方の定理を利用。しかしなぜか近づくと距離が遠くなるという動作になるので逆数を取る
    }

    // 計測したIRボールの角度・距離をセンサモニターに表示
    Serial.print("IRedge : ");
    Serial.print(IRedge, 1); // 小数点1桁で表示
    Serial.print("\tIRdistance : ");
    Serial.println(IRdist, 1); // 小数点1桁で表示

    // 待つ
    delay(Update_Ms); // Update_Ms分待つ
}