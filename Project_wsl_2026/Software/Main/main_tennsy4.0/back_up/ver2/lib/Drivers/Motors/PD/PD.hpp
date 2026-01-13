#pragma once

#include <Arduino.h>
#include "Functions.hpp"

class PD
{
private:
    double _kp = 0.0; // 比例ゲイン
    double _kd = 0.0; // 微分ゲイン

    Timer _timer;          // D制御用のタイマー
    int _value = 0;        // 現在の値[-180~180]
    int _oldvalue = 0;     // 昔の値[-180~180]
    int _gap_of_value = 0; // 値の変化量

    double _p_power = 0.0; // P制御の出力
    double _d_power = 0.0; // D制御の出力
    double _output = 0;    // 出力
public:
    // 関数・コンストラクタ
    PD(double kp,double kd);
    void process(int deg, int target);

    // [-100~100]の値を返す
    int output()
    {
        return (int)round(_output);
    }
};