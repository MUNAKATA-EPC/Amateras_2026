#include "LINExiao.hpp"

LINExiao::LINExiao(HardwareSerial *serial)
{
    _serial = serial;
}

void LINExiao::begin(int baudrate)
{
    _baudrate = baudrate;
    (*_serial).begin(_baudrate);
}

void LINExiao::update()
{
    if ((*_serial).available())
    {
        // 情報を10進数で受信->2進数に直す方法
        int data_10 = (*_serial).readStringUntil('\0').toInt(); // 16+3個のデータを受信

        int shift_num; // 解析に使う変数
        for (int i = 0; i < 19; i++)
        {
            shift_num = 1 << i; // 0001を左にiだけ移動させる

            if ((shift_num & data_10) > 0) // shift_numとdata_10の論理積が0より大きいならば
                _data[i] = true;
            else
                _data[i] = false;
        }
    }
}

bool LINExiao::isExist()
{
    return _exist;
}

bool LINExiao::getSideRight()
{
    return _data[16];
}

bool LINExiao::getSideLeft()
{
    return _data[17];
}

bool LINExiao::getSideBack()
{
    return _data[18];
}

int LINExiao::getData(int pin)
{
    return _data[pin];
}