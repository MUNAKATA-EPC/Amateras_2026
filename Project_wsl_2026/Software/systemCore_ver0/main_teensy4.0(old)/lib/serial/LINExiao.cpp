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
        char buf[20];                                                        // 多めにとる、受信したデータを格納する変数
        int buf_long = (*_serial).readBytesUntil('a', buf, sizeof(buf) - 1); // ↑'a'は読まないので7文字受信 000000a
        buf[buf_long] = '\0';                                                // 終端記号を追加

        // 受信したデータが6文字なら正常なデータである
        if (buf_long == 6)
        {
            if (buf[0] == '-')
            {
                _deg = -1;
            }
            else
            {
                _deg = char_to_int(buf, 0, 2); // 0番目から2番目までの数をint型で読み取る
            }

            // サイドの状態を取得
            _side_right = char_to_int(buf, 3, 3);
            _side_left = char_to_int(buf, 4, 4);
            _side_back = char_to_int(buf, 5, 5);

            if (_side_right || _side_left || _side_back || _deg != -1)
            {
                _exist = true;
            }
            else
            {
                _exist = false;
            }
        }
    }
}

bool LINExiao::isExist()
{
    return _exist;
}

bool LINExiao::getSideRight()
{
    return _side_right;
}

bool LINExiao::getSideLeft()
{
    return _side_left;
}

bool LINExiao::getSideBack()
{
    return _side_back;
}

int LINExiao::getDeg()
{
    return _deg;
}