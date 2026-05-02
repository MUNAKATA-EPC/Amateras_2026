#include "pd.hpp"

PD::PD(float kp, float kd, float lpf_alpha)
    : _kp(kp), _kd(kd), _lpf_alpha(lpf_alpha)
{
    _last_time = micros();
}

void PD::setGains(float kp, float kd)
{
    _kp = kp;
    _kd = kd;
}

void PD::setLPF(float alpha)
{
    _lpf_alpha = constrain(alpha, 0.01f, 1.0f);
}

void PD::setDeadband(float band)
{
    _deadband = fabsf(band);
}

void PD::useP(bool use) { _useP = use; }
void PD::useD(bool use) { _useD = use; }

void PD::process(float val, float target, bool angle)
{
    uint32_t now = micros();
    float dt = (float)(now - _last_time) / 1000000.0f; // 秒単位に変換
    if (dt <= 0)
        dt = 0.001f; // ゼロ除算防止
    _last_time = now;

    _value = val;

    float error = angle ? getDiffDeg(target, _value) : (target - _value);

    if (fabsf(error) < _deadband)
        error = 0.0f;
    _p_power = error * _kp;

    float diff = angle ? getDiffDeg(_value, _old_value) : (_value - _old_value);

    float raw_d_speed = diff / dt;
    float raw_d_power = raw_d_speed * _kd;

    _d_filtered = (_lpf_alpha * raw_d_power) + ((1.0f - _lpf_alpha) * _d_filtered);
    _d_power = _d_filtered;

    _old_value = _value;

    float total_power = 0.0f;
    if (_useP)
        total_power += _p_power;
    if (_useD)
        total_power += _d_power; // Kdを負の値で渡している場合は加算、正なら減算

    _output = constrain(total_power, -100.0f, 100.0f);
}

float PD::output() const
{
    return _output;
}

void PD::reset(float current_val)
{
    _old_value = current_val;
    _d_filtered = 0.0f;
    _output = 0.0f;
    _last_time = micros();
}

float PD::getDiffDeg(float target, float current)
{
    float diff = target - current;
    while (diff > 180.0f)
        diff -= 360.0f;
    while (diff < -180.0f)
        diff += 360.0f;
    return diff;
}