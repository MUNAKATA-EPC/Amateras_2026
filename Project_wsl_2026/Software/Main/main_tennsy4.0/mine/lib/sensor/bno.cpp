#include "bno.hpp"

static Adafruit_BNO055 *_bno = nullptr;

static int _deg_normal = 0;
static int _deg_reset = 0;
static int _deg = 0;

bool bnoInit(TwoWire *wire, uint8_t address)
{
    if (_bno != nullptr)
    {
        delete _bno;
        _bno = nullptr;
    }
    _bno = new Adafruit_BNO055(BNO055_ID, address, wire);

    // Teensy 4.0は高速なため、BNOの起動を確実に待つ
    Timer timer;
    timer.reset();
    bool success = false;
    while (!success && timer.msTime() < 1000UL)
    {
        success = _bno->begin(OPERATION_MODE_IMUPLUS);
        if (!success)
            delay(50);
    }

    if (success)
    {
        _bno->setExtCrystalUse(true);
    }
    return success;
}

void bnoUpdate(bool resetbtn)
{
    if (_bno == nullptr)
        return;

    imu::Vector<3> euler = _bno->getVector(Adafruit_BNO055::VECTOR_EULER);
    float current_raw_x = (float)euler.x();
    _deg_normal = (int)current_raw_x;

    if (resetbtn)
    {
        _deg_reset = _deg_normal;
    }

    _deg = (_deg_normal - _deg_reset + 360) % 360;
    if (_deg > 180)
        _deg -= 360;
}

int bnoDeg()
{
    return _deg;
}