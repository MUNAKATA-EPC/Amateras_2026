#include "M5stamp.hpp"

M5stamp::M5stamp(HardwareSerial *serial, uint32_t baudrate, uint8_t frameHeader)
{
    _serial = serial;
    _baudrate = baudrate;
    _frameHeader = frameHeader;
}

void M5stamp::begin()
{
    _serial->begin(_baudrate);
}

int16_t M5stamp::readSerial()
{
    uint8_t low = (uint8_t)_serial->read();
    uint8_t high = (uint8_t)_serial->read();
    return (int16_t)((high << 8) | low);
}

void M5stamp::update()
{
    while (_serial->available() >= 11)
    {
        while (_serial->available() > 0 && _serial->peek() != _frameHeader)
        {
            _serial->read();
        }

        if (_serial->available() < 11)
            break;

        if (_serial->peek() == _frameHeader)
        {
            _serial->read();

            int16_t deg, dis;

            // 左ステック
            deg = readSerial();
            LeftStick.setAngle(deg, 0, deg != -1);

            // 右ステック
            deg = readSerial();
            dis = readSerial();
            RightStick.setAngle(deg, dis, deg != -1);
        }
        else
        {
            _serial->read();
        }
    }
}