#pragma once

#include <Arduino.h>
#include "motorsDSR1202.hpp"
#include "pd.hpp"
#include "vector.hpp"

bool motorsInit(HardwareSerial *serial, uint32_t baudrate);
void motorsSetTogglePin(uint8_t pin, uint8_t pinmode);                                 // モータの起動トグルスイッチのピン設定
void motorsSetDegPosition(float deg_1ch, float deg_2ch, float deg_3ch, float deg_4ch); // モータの位置をセット
void motorsSetMoveSign(int sign_1ch, int sign_2ch, int sign_3ch, int sign_4ch);        // 移動のための符号をセット
void motorsSetPdSign(int sign_1ch, int sign_2ch, int sign_3ch, int sign_4ch);          // PD制御のための符号をセット
// 制御系
void motorsStop();
void motorsDirectMove(int value_1ch, int value_2ch, int value_3ch, int value_4ch);
void motorsPdProcess(PD *pd, float deg, float target); // PD出力範囲は-100~100
void motorsMove(float deg, float power);               // PD出力範囲は-100~100
void motorsVectorMove(Vector *vec);                    // PD出力範囲は-100~100
void motorsPdMove();                                   // PD出力範囲は-100~100