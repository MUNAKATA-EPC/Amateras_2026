#include "BNO055.hpp"

BNO055::BNO055(TwoWire *wire, uint8_t address, Button *resetbtn)
{
    if (_bno != nullptr)
    {
        delete _bno;
    }
    _bno = new Adafruit_BNO055(BNO055_ID, address, wire);

    _resetbtn = resetbtn;
}

void BNO055::begin()
{
    Timer bnoTimer;
    bnoTimer.reset();
    while (!_bno->begin(OPERATION_MODE_IMUPLUS) && bnoTimer.msTime() < 5000)
    {
        delay(10); // BNO055の通信開始待ち
    }

    if (!_bno->begin(OPERATION_MODE_IMUPLUS))
    {
        Serial.println("bno timeout");
    }

    _bno->setExtCrystalUse(true); // 外部水晶振動子使用

    _resetbtn->begin();
}

void BNO055::update()
{
    imu::Vector<3> euler = _bno->getVector(Adafruit_BNO055::VECTOR_EULER);
    _degNormal = euler.x(); // X軸の角度

    _resetbtn->update();

    if (_resetbtn->isReleased()) // リセットボタンが押されたら
        _degReset = _degNormal;

    // 補正後の角度
    _deg = (int)((_degNormal - _degReset + 360) % 360);
}
