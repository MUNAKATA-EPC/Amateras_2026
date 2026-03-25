#pragma once
#include <Arduino.h>

class PD
{
public:
    PD(float kp, float kd, float lpf_alpha = 0.2f);

    // 設定変更用
    void setGains(float kp, float kd);
    void setLPF(float alpha);
    void setDeadband(float band); // 誤差がこの値以下なら出力を0にする

    void useP(bool use);
    void useD(bool use);

    void process(float val, float target, bool angle = false);

    float output() const;
    void reset(float current_val);

private:
    float _kp, _kd;
    float _lpf_alpha;
    float _deadband = 0.0f;

    float _value = 0.0f;
    float _old_value = 0.0f;
    float _p_power = 0.0f;
    float _d_power = 0.0f;
    float _d_filtered = 0.0f;
    float _output = 0.0f;

    bool _useP = true;
    bool _useD = true;

    uint32_t _last_time = 0;

    // 角度の差分計算
    float getDiffDeg(float a, float b);
};