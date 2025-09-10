#pragma once

#include <Arduino.h>
#include "Device/DSR1202.hpp"
#include "PD/PD.hpp"
#include "Functions.hpp"
#include "Sensors.hpp"

class Motors
{
private:
    DSR1202 *_dsr = nullptr;
    int _deg_position[4] = {0};     // モータの位置
    int _move_sign[4] = {1};        // モータの移動についての符号
    int _pd_sign[4] = {1};          // モータのPD制御についての符号
    double _move_output[4] = {0.0}; // 移動に関する出力
    int _output[4] = {0};           // 最終出力

    PD *_pd;

public:
    // 関数・コントラクタ
    Motors(HardwareSerial *serial, uint32_t baudrate);
    void begin(DigitalSensor *toggle);
    void setDegPosition(int deg_1ch, int deg_2ch, int deg_3ch, int deg_4ch);  // モータの位置をセット
    void setMoveSign(int sign_1ch, int sign_2ch, int sign_3ch, int sign_4ch); // 移動のための符号をセット
    void setPdSign(int sign_1ch, int sign_2ch, int sign_3ch, int sign_4ch);   // PD制御のための符号をセット
    // 制御系
    void stop();
    void PDprocess(PD *pd, int deg, int target); // PD出力範囲は-100~100
    void move(int deg, int power);               // PD出力範囲は-100~100
    void PDmove();                               // PD出力範囲は-100~100
};