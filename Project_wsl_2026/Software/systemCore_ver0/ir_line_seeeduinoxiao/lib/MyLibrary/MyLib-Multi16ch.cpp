#include <Arduino.h>
#include "MySettings.h"

void MULTI16begin()
{
    pinMode(MULTI16_S0PIN, OUTPUT); // S0ピンを出力に設定
    pinMode(MULTI16_S1PIN, OUTPUT); // S1ピンを出力に設定
    pinMode(MULTI16_S2PIN, OUTPUT); // S2ピンを出力に設定
    pinMode(MULTI16_S3PIN, OUTPUT); // S3ピンを出力に設定
    pinMode(MULTI16_COMPIN, INPUT); // COMピンを入力に設定
}

int MULTI16read(int MULTI16pinNum)
{
    // S0〜S3ピンに信号を送る（ビット演算）
    digitalWrite(MULTI16_S0PIN, (MULTI16pinNum & 0x01) ? HIGH : LOW); // 最下位ビット
    digitalWrite(MULTI16_S1PIN, (MULTI16pinNum & 0x02) ? HIGH : LOW);
    digitalWrite(MULTI16_S2PIN, (MULTI16pinNum & 0x04) ? HIGH : LOW);
    digitalWrite(MULTI16_S3PIN, (MULTI16pinNum & 0x08) ? HIGH : LOW); // 4ビット目

    delayMicroseconds(MULTI16_WAITMS); // マルチプレクサの切り替え待ち

    return analogRead(MULTI16_COMPIN);
}