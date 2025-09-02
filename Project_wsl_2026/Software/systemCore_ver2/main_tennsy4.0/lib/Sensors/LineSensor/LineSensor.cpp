#include "LineSensor.hpp"

void LineSensor::init(HardwareSerial *serial, uint32_t baudrate, uint8_t frameHeader)
{
    _serial = serial;
    _baudrate = baudrate;
    _frameHeader = frameHeader;

    _serial->begin(_baudrate);

    // 初期化
    _detected = false;
    _deg = _degMemory = -1;
    _dis = -1;
    _sideRight = _sideLeft = _sideBack = false;

    for (int i = 0; i < 19; i++)
        _sensor[i] = _sensorMemory[i] = false;
}

int LineSensor::degCompute(bool *data)
{
    if (!_detected)
        return -1;

    _x = 0;
    _y = 0;
    for (int i = 0; i < 16; i++)
    {
        if (data[i])
        {
            _x += cos(radians(22.5 * i));
            _y += sin(radians(22.5 * i));
        }
    }

    if (_sideRight)
    {
        _x += cos(radians(90));
        _y += sin(radians(90));
    }
    if (_sideLeft)
    {
        _x += cos(radians(270));
        _y += sin(radians(270));
    }
    if (_sideBack)
    {
        _x += cos(radians(180));
        _y += sin(radians(180));
    }

    return (int)round(degrees(atan2(_y, _x)));
}

void LineSensor::update()
{
    // データの読み取り
    while (_serial->available() >= 4)
    {
        while (_serial->available() > 0 && _serial->peek() != _frameHeader)
            _serial->read();

        if (_serial->available() < 4)
            break;

        if (_serial->peek() == _frameHeader)
        {
            _serial->read(); // ヘッダー読み捨て

            uint8_t low = _serial->read();
            uint8_t middle = _serial->read();
            uint8_t high = _serial->read();

            uint32_t bit_mask = ((uint32_t)high << 16) | ((uint32_t)middle << 8) | (uint32_t)low;
            _detected = (bit_mask > 0);

            for (int i = 0; i < 19; i++)
                _sensor[i] = (bit_mask & (1UL << i)) != 0;

            _sideRight = _sensor[16];
            _sideLeft = _sensor[17];
            _sideBack = _sensor[18];
        }
        else
        {
            _serial->read();
        }
    }

    // データの加工
    if (_detected)
    {
        // 瞬間角度・距離
        _deg = degCompute(_sensor);
        if (_deg < 0)
            _deg += 360;
        _dis = sqrt(_x * _x + _y * _y);

        // 記憶角度
        for (int i = 0; i < 19; i++)
        {
            if (_sensor[i])
                _sensorMemory[i] = true;
        }

        _degMemory = degCompute(_sensorMemory);
        if (_degMemory < 0)
            _degMemory += 360;
    }
    else
    {
        // 初期化
        _deg = _degMemory = -1;
        _dis = -1;
        for (int i = 0; i < 19; i++)
            _sensorMemory[i] = false;
    }
}
