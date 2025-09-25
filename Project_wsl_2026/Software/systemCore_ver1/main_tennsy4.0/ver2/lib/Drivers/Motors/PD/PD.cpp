#include "PD.hpp"

// #define D_USE_TIMER //D制御でタイマーを使うか

PD::PD(double kp, double kd)
{
    _kp = kp;
    _kd = kd;
}

void PD::process(int deg, int target)
{
    deg = (deg + 360 + target) % 360;
    _value = (deg < 180.0) ? deg : deg - 360.0;

    _gap_of_value = _value - _oldvalue;
    if (_gap_of_value > 180.0)
        _gap_of_value -= 360.0;
    if (_gap_of_value < -180.0)
        _gap_of_value += 360.0;

    _oldvalue = _value;

    // P制御
    _p_power = _value * _kp;

    // D制御
    _d_power = _kd * _gap_of_value;
#ifdef D_USE_TIMER
    double sec = 0.0;
    if (!_timer.everCalled())
    {
        _timer.reset();
        _d_power = 0.0;
    }
    else
    {
        sec = _timer.msTime() / 1000.0;
        if (sec > 1e-6)
            _d_power = _kd * _gap_of_value / sec;
        else
            _d_power = 0.0;
        _timer.reset();
    }
#endif

    _output = constrain(_p_power + _d_power, -100.0, 100.0);
}
