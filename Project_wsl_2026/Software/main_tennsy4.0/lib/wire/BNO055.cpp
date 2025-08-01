#include "BNO055.hpp"

BNO055::BNO055(Adafruit_BNO055 *bno)
{
    _bno = bno;
}

void BNO055::setresetPin(int pin, int pinmode)
{
    _reset_pin = pin;
    _reset_pinmode = pinmode;

    pinMode(_reset_pin, _reset_pinmode); // ピンを設定
}

void BNO055::begin()
{
    int now_time = millis(); // タイマーをセット
    while (!(*_bno).begin(OPERATION_MODE_IMUPLUS) && (millis() - now_time) <= 5000)
    {
        delay(10); // 10ms待機
    }

    (*_bno).setExtCrystalUse(true); // セットする
}

void BNO055::update()
{
    imu::Vector<3> euler = (*_bno).getVector(Adafruit_BNO055::VECTOR_EULER); // BNO055からデータを取得

    _normal_deg = euler.x(); // X軸のデータを取得

    if (_reset_pinmode == INPUT_PULLDOWN)
    {
        if (digitalRead(_reset_pin) == HIGH)
        {
            _reset_deg = _normal_deg; // _reset_degを更新
        }
    }
    else
    {
        if (digitalRead(_reset_pin) == LOW)
        {
            _reset_deg = _normal_deg; // _reset_degを更新
        }
    }

    _deg = (int)(_normal_deg - _reset_deg + 360) % 360; // 角度を計算
}

int BNO055::getDeg()
{
    return _deg; //_degを返す
}