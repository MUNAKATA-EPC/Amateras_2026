#include "BNO055.hpp"

Button resetButton; // ジャイロのリセット用ボタン

void BNO055::init(TwoWire *wire, uint8_t address, uint8_t pin, uint8_t pinmode)
{
    if (_bno != nullptr)
    {
        delete _bno;
    }
    _bno = new Adafruit_BNO055(BNO055_ID, address, wire);

    Timer bnoTimer;
    bnoTimer.reset();
    while (!_bno->begin(OPERATION_MODE_IMUPLUS) && bnoTimer.millis() < 5000)
    {
        delay(10); // BNO055の通信開始待ち
    }

    _bno->setExtCrystalUse(true); // 外部水晶振動子使用

    resetButton.init(pin, pinmode);
}

void BNO055::update()
{
    imu::Vector<3> euler = _bno->getVector(Adafruit_BNO055::VECTOR_EULER);
    _degNormal = euler.x(); // X軸の角度

    if (resetButton.isReleased()) // リセットボタンが押されたら
        _degReset = _degNormal;

    // 補正後の角度 (-180〜180)
    _deg = (int)((_degNormal - _degReset + 360) % 360);
}
