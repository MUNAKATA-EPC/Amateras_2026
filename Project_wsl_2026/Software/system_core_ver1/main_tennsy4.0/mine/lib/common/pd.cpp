#include "pd.hpp"

// #define D_USE_TIMER // ループが短すぎてうまくいかないです

PD::PD(float kp, float kd)
{
    // PゲインとDゲインを初期化
    _kp = kp;
    _kd = kd;

    // 過去の値とタイマー関連の初期化
    _oldvalue = 0.0f;
    _output = 0.0f;

#ifdef D_USE_TIMER
    _timer.reset();
#endif
}

void PD::process(float val, float target, bool angle)
{
    // 現在値を設定
    _value = val;
    // P制御（比例項）の計算
    // 誤差を計算 (目標 - 現在値)
    float error = angle ? (float)diffDeg(target, _value) : float(target - _value);

    // P制御の計算
    _p_power = error * _kp;

    // D制御（微分項）の計算
    // 角度の変化量 (_gap_of_value) を計算
    _gap_of_value = angle ? (float)diffDeg(_value, _oldvalue) : float(_value - _oldvalue);

#ifdef D_USE_TIMER
    float delta = 0.0f;
    if (!_timer.everCalled())
    {
        _timer.reset();
        _d_power = 0.0f;
    }
    else
    {
        delta = _timer.msTime() * 10UL;

        // ゼロ除算を避ける
        if (delta > 1e-6)
            _d_power = _kd * _gap_of_value / delta; // D = Kd * (変化量 / 時間)
        else
            _d_power = 0.0f;

        _timer.reset();
    }
#endif

#ifndef D_USE_TIMER
    // D項の計算（通常は変化量にKdをかける）
    _d_power = _kd * _gap_of_value;
#endif

    // 過去の値を更新
    _oldvalue = _value;

    // P項とD項を合計し、出力を -100.0 から 100.0 の範囲に制限
    float power = 0.0f;
    power += (_useP) ? _p_power : 0.0f;
    power += (_useD) ? _d_power : 0.0f; // D成分は動きを抑える作用
    _output = constrain(power, -100.0f, 100.0f);
}

PD pd_gyro(0.65, -4.5); // 共通で使えるようにしておく
PD pd_cam(0.65, -4.5);  // 共通で使えるようにしておく