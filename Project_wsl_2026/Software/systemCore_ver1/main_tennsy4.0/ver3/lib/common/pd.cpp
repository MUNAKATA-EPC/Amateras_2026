#include "pd.hpp"

PD::PD(double kp, double kd)
{
    // PゲインとDゲインを初期化
    _kp = kp;
    _kd = kd;

    // 過去の値とタイマー関連の初期化
    _oldvalue = 0;
    _output = 0.0;

#ifdef D_USE_TIMER
    _timer.reset();
#endif
}

void PD::process(double val, double target)
{
    // 現在値を設定
    _value = val;
    // --- P制御（比例項）の計算 ---
    // 誤差を計算 (目標 - 現在値)
    double error = (double)target - _value;

    // --- P制御の計算 ---
    _p_power = error * _kp;

    // --- D制御（微分項）の計算 ---
    // 角度の変化量 (_gap_of_value) を計算
    _gap_of_value = _value - _oldvalue;

    // 過去の値を更新
    _oldvalue = _value;

    // D項の計算（通常は変化量にKdをかける）
    _d_power = _kd * _gap_of_value;

#ifdef D_USE_TIMER
    // D制御にタイマーを使用する場合（真の微分項、単位時間あたりの変化量）
    double sec = 0.0;
    if (!_timer.everCalled())
    {
        _timer.reset();
        _d_power = 0.0;
    }
    else
    {
        sec = _timer.msTime() / 1000.0;

        // ゼロ除算を避ける
        if (sec > 1e-6)
            _d_power = _kd * _gap_of_value / sec; // D = Kd * (変化量 / 時間)
        else
            _d_power = 0.0;

        _timer.reset();
    }
#endif

    // P項とD項を合計し、出力を -100.0 から 100.0 の範囲に制限
    _output = constrain(_p_power + _d_power, -100.0, 100.0);
}