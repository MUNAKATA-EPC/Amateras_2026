#pragma once

#include <Arduino.h>
#include "timer.hpp"
#include "angleHelper.hpp"

class PD
{
private:
    float _kp = 0.0f; // 比例ゲイン
    float _kd = 0.0f; // 微分ゲイン

    bool _useP = true;
    bool _useD = true;

    uint32_t old_micro_time = 0xFFFFFFFF; // 昔に時間記録用で最初は最大値を格納しておく（ミクロセカンド）

    float _value = 0.0f;        // 現在の値
    float _oldvalue = 0.0f;     // 昔の値
    float _gap_of_value = 0.0f; // 値の変化量

    float _p_power = 0.0f; // P制御の出力
    float _d_power = 0.0f; // D制御の出力
    float _output = 0.0f;  // 出力
public:
    // 関数・コンストラクタ
    PD(float kp, float kd);

    void useP(bool use);
    void useD(bool use);

    void process(float val, float target, bool angle);

    // -100.0f～100.0fの値を返す
    float const output();

    // リセットしD成分の暴走を防ぐ
    void reset(float current_val);
};

extern PD pd_gyro; // 共通で使えるようにしておく
extern PD pd_cam;  // 共通で使えるようにしておく