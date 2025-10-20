#pragma once

#include <Arduino.h>
#include "timer.hpp"

class PD
{
private:
    double _kp = 0.0; // 比例ゲイン
    double _kd = 0.0; // 微分ゲイン

    Timer _timer;               // D制御用のタイマー
    double _value = 0.0;        // 現在の値
    double _oldvalue = 0.0;     // 昔の値
    double _gap_of_value = 0.0; // 値の変化量

    double _p_power = 0.0; // P制御の出力
    double _d_power = 0.0; // D制御の出力
    double _output = 0;    // 出力
public:
    // 関数・コンストラクタ
    PD(double kp, double kd);
    void process(double val, double target);

    // [-100.0~100.0]の値を返す
    double const output()
    {
        return _output;
    }
};