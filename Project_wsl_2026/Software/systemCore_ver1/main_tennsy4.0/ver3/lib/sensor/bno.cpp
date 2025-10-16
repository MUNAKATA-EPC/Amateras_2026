#include "bno.hpp"
inline Adafruit_BNO055 *_bno = nullptr;

inline int _degNormal = 0; // BNO055からの生角度
inline int _degReset = 0;  // リセット時の角度
inline int _deg = 0;       // 補正後の角度格納用

bool bnoInit(TwoWire *wire, uint8_t address)
{
    if (_bno != nullptr)
    {
        delete _bno;
    }
    _bno = new Adafruit_BNO055(BNO055_ID, address, wire);

    Timer timer;
    timer.reset();
    bool success = _bno->begin(OPERATION_MODE_IMUPLUS);
    while (!success && timer.msTime() < 5000)
    {
        delay(10); // BNO055の通信開始待ち
    }
    _bno->setExtCrystalUse(true); // 外部水晶振動子使用

    return success;
}

void bnoUpdate(bool resetbtn)
{
    imu::Vector<3> euler = _bno->getVector(Adafruit_BNO055::VECTOR_EULER);
    _degNormal = euler.x(); // X軸の角度

    if (resetbtn) // リセットボタンが押されたら
        _degReset = _degNormal;

    // 補正後の角度
    _deg = (int)((_degNormal - _degReset + 360) % 360);
    if (_deg > 180)
        _deg -= 360; // -180~180に変換
}

int bnoDeg() { return _deg; }
