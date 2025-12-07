#pragma once

#include <Arduino.h>
#include "timer.hpp"
#include "angle_helper.hpp"

class PD
{
private:
    float _kp = 0.0f; // 比例ゲイン
    float _kd = 0.0f; // 微分ゲイン

    bool _useP = true;
    bool _useD = true;

    Timer _timer;               // D制御用のタイマー
    float _value = 0.0f;        // 現在の値
    float _oldvalue = 0.0f;     // 昔の値
    float _gap_of_value = 0.0f; // 値の変化量

    float _p_power = 0.0f; // P制御の出力
    float _d_power = 0.0f; // D制御の出力
    float _output = 0.0f;    // 出力
public:
    // 関数・コンストラクタ
    PD(float kp, float kd);

    void useP(bool use)
    {
        _useP = use;
    }
    void useD(bool use)
    {
        _useD = use;
    }

    void process(float val, float target, bool angle);

    // [-100.0f~100.0f]の値を返す
    float const output()
    {
        return _output;
    }
};