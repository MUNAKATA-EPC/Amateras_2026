#pragma once

#include <Arduino.h>
#include "motorsDsr1202.hpp"
#include "motorsPd.hpp"
#include "vector.hpp"

bool motorsInit(HardwareSerial *serial, uint32_t baudrate);
void motorsSetDegPosition(int deg_1ch, int deg_2ch, int deg_3ch, int deg_4ch);  // モータの位置をセット
void motorsSetMoveSign(int sign_1ch, int sign_2ch, int sign_3ch, int sign_4ch); // 移動のための符号をセット
void motorsSetPdSign(int sign_1ch, int sign_2ch, int sign_3ch, int sign_4ch);   // PD制御のための符号をセット
// 制御系
void motorsStop();
void motorsPdProcess(PD *pd, int deg, int target); // PD出力範囲は-100~100
void motorsMove(int deg, int power);               // PD出力範囲は-100~100
void motorsPdMove();                               // PD出力範囲は-100~100

extern PD *pdGyro; // ジャイロ用のPD調節値
extern PD *pdCam;  // カメラ用のPD調節値