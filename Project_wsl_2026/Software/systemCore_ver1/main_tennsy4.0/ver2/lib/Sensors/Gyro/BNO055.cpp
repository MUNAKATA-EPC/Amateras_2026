#include "BNO055.hpp"

BNO055::BNO055(TwoWire *wire, uint8_t address)
{
    if (_bno != nullptr)
    {
        delete _bno;
    }
    _bno = new Adafruit_BNO055(BNO055_ID, address, wire);
}

bool BNO055::begin()
{
    Timer bnoTimer;
    bnoTimer.reset();
    bool success = _bno->begin(OPERATION_MODE_IMUPLUS);
    while (!success && bnoTimer.msTime() < 5000)
    {
        delay(10); // BNO055の通信開始待ち
    }
    _bno->setExtCrystalUse(true); // 外部水晶振動子使用

    return success;
}

void BNO055::update(bool resetbtn)
{
    imu::Vector<3> euler = _bno->getVector(Adafruit_BNO055::VECTOR_EULER);
    _degNormal = euler.x(); // X軸の角度

    if (resetbtn) // リセットボタンが押されたら
        _degReset = _degNormal;

    // 補正後の角度
    _deg = (int)((_degNormal - _degReset + 360) % 360);
}
