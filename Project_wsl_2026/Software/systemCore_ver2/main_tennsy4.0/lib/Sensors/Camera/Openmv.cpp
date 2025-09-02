#include "Openmv.hpp"

void Openmv::init(HardwareSerial *serial, uint32_t baudrate, uint8_t frameHeader)
{
    _serial = serial;
    _baudrate = baudrate;
    _frameHeader = frameHeader;

    _serial->begin(_baudrate);
}

int16_t Openmv::readAndDocking()
{
    uint8_t low = (uint8_t)_serial->read();
    uint8_t high = (uint8_t)_serial->read();
    return (int16_t)((high << 8) | low);
}

void Openmv::update()
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

            // コートの角度
            int16_t deg, dis;
            deg = readAndDocking();
            field.set(deg, 0);

            // 黄色ゴールの角度・距離
            deg = readAndDocking();
            dis = readAndDocking();
            yellowGoal.set(deg, dis);

            // 青色ゴールの角度・距離
            deg = readAndDocking();
            dis = readAndDocking();
            blueGoal.set(deg, dis);
        }
        else
        {
            _serial->read();
        }
    }
}
