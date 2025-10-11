#include "LineSensor.hpp"

LineSensor::LineSensor(HardwareSerial *serial, uint32_t baudrate, uint8_t frameHeader)
{
    _serial = serial;
    _baudrate = baudrate;
    _frameHeader = frameHeader;

    // 初期化
    _ringDetected = _sideDetected = false;
    _ringDeg = _sideDeg = -1;
    _dis = -1;
    _sideRight = _sideLeft = _sideBack = false;
    _x = _y = 0.0;

    for (int i = 0; i < 19; i++)
        _sensor[i] = false;
}

void LineSensor::begin()
{
    _serial->begin(_baudrate);
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

            // エンジェルの検出
            _ringDetected = false;

            for (int i = 0; i < 19; i++)
            {
                _sensor[i] = (bit_mask & (1UL << i)) != 0;
                if (_sensor[i] == true)
                    _ringDetected = true;
            }

            // 　サイドの検出
            _sideLeft = _sensor[16];
            _sideRight = _sensor[17];
            _sideBack = _sensor[18];

            _sideDetected = _sideLeft || _sideRight || _sideBack;
        }
        else
        {
            _serial->read();
        }
    }

    // 計算
    
}
