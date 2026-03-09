#include <Arduino.h>
#include "MyLib-Multi16ch.h"
#include "MySettings.h"

void setup()
{
    // シリアルの初期設定
    Serial.begin(9600); // PCとの通信開始。ボートレートは9600とする。

    // マルチプレクサの初期設定
    MULTI16begin();
}

void loop()
{
    // マルチプレクサの読み取り例
    for (int i = 0; i < 16; i++)
    {
        int value = MULTI16read(i); // "i"ピンの値を読み取る
        Serial.print(" Pin ");
        Serial.print(i);
        Serial.print(": ");
        Serial.print(value); // 読み取った値をシリアルモニタに出力
    }
    Serial.println(""); // 改行

    delay(100); // 100ms待つ
}