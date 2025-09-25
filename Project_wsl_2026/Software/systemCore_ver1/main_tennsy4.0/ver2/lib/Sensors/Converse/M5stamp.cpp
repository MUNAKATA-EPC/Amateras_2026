#include "M5stamp.hpp"

M5stamp::M5stamp(HardwareSerial *serial, uint32_t baudrate, uint8_t frameHeader)
{
    _serial = serial;
    _baudrate = baudrate;
    _frameHeader = frameHeader;
}

void M5stamp::begin(int left_adjust, int right_adjust)
{
    _serial->begin(_baudrate);

    _left_adjust = left_adjust;
    _right_adjust = right_adjust;
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
            _serial->read(); // ヘッダーを捨てる

            // ステックのx,y
            _stick_lx = (int8_t)_serial->read(); // 左ステックX (-128~127)
            if (abs(_stick_lx) < abs(_left_adjust))
                _stick_lx = 0;
            _stick_ly = -(int8_t)(_serial->read()); // 左ステックY
            if (abs(_stick_ly) < abs(_left_adjust))
                _stick_ly = 0;
            _stick_rx = (int8_t)_serial->read(); // 右ステックX
            if (abs(_stick_rx) < abs(_right_adjust))
                _stick_rx = 0;
            _stick_ry = -(int8_t)(_serial->read()); // 右ステックY
            if (abs(_stick_ry) < abs(_right_adjust))
                _stick_ry = 0;

            Serial.println(_stick_ry);

            // ステックの角度・距離
            if (_stick_lx == 0 && _stick_ly == 0)
            {
                _leftstick_detected = false;
                _leftstick_deg = -1;
                _leftstick_dis = -1;
            }
            else
            {
                _leftstick_detected = true;

                double _deg_sub = degrees(atan2(_stick_ly, _stick_lx)); // 角度を算出
                _deg_sub = (_deg_sub < 0) ? _deg_sub + 360 : _deg_sub;
                _leftstick_deg = (int)round(_deg_sub);
                _leftstick_deg = (_leftstick_deg - 90 + 360) % 360; // 調整

                _leftstick_dis = sqrt(_stick_lx * _stick_lx + _stick_ly * _stick_ly); // 距離を算出
                _leftstick_dis = constrain(_leftstick_dis, 0, 128);
            }
            if (_stick_rx == 0 && _stick_ry == 0)
            {
                _rightstick_detected = false;
                _rightstick_deg = -1;
                _rightstick_dis = -1;
            }
            else
            {
                _rightstick_detected = true;

                double _deg_sub = degrees(atan2(_stick_ry, _stick_rx)); // 角度を算出
                _deg_sub = (_deg_sub < 0) ? _deg_sub + 360 : _deg_sub;
                _rightstick_deg = (int)round(_deg_sub);
                _rightstick_deg = (_rightstick_deg - 90 + 360) % 360; // 調整

                _rightstick_dis = sqrt(_stick_rx * _stick_rx + _stick_ry * _stick_ry); // 距離を算出
                _rightstick_dis = constrain(_rightstick_dis, 0, 128);
            }

            // ボタン
            uint8_t low = _serial->read();                // ボタン下位バイト
            uint8_t high = _serial->read();               // ボタン上位バイト
            button_bitmask = (uint16_t(high) << 8) | low; // 16bit にまとめる
        }
        else
        {
            _serial->read();
        }
    }
}