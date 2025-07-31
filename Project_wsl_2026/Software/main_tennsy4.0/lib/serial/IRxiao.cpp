#include "IRxiao.hpp"

IRxiao::IRxiao(HardwareSerial *serial)
{
    _serial = serial;
}

void IRxiao::begin(int baudrate)
{
    _baudrate = baudrate;
    (*_serial).begin(_baudrate);
    (*_serial).setTimeout(10);
}

void IRxiao::update()
{
    if ((*_serial).available() > 0)
    {
        _deg = (*_serial).readStringUntil('a').toInt();      // 'a'まで読む
        _distance = (*_serial).readStringUntil('b').toInt(); //'b'まで読む
        if (_deg == -1)
            _exist = false;
        else
            _exist = true;
    }
}

bool IRxiao::isExist()
{
    return _exist;
}

int IRxiao::getDeg()
{
    return _deg;
}

int IRxiao::getDistance()
{
    return _distance;
}
