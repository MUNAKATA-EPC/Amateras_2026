#include "Openmv.hpp"

Openmv::Openmv(HardwareSerial *serial, uint32_t baudrate, uint8_t frameHeader)
{
    _serial = serial;
    _baudrate = baudrate;
    _frameHeader = frameHeader;
}

bool Openmv::begin()
{
    _serial->begin(_baudrate);

    return _serial->available() > 0;
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
            _serial->read(); // ヘッダーを捨てる

            // コートの角度
            uint8_t low1 = _serial->read();                                // ボールの角度の下位バイトを読み取る
            uint8_t high1 = _serial->read();                               // ボールの角度の上位バイトを読み取る
            _field_deg = int16_t((uint16_t(high1) << 8) | uint16_t(low1)); // 上位バイトと下位バイトをつなげる
            _field_detected = (_field_deg != -1);

            // 黄色ゴールの角度・距離
            uint8_t low2 = _serial->read();                                     // ボールの角度の下位バイトを読み取る
            uint8_t high2 = _serial->read();                                    // ボールの角度の上位バイトを読み取る
            _yellowgoal_deg = int16_t((uint16_t(high2) << 8) | uint16_t(low2)); // 上位バイトと下位バイトをつなげる
            uint8_t low3 = _serial->read();                                     // ボールの角度の下位バイトを読み取る
            uint8_t high3 = _serial->read();                                    // ボールの角度の上位バイトを読み取る
            _yellowgoal_dis = int16_t((uint16_t(high3) << 8) | uint16_t(low3)); // 上位バイトと下位バイトをつなげる
            _yellowgoal_detected = (_yellowgoal_deg != -1);

            // 青色ゴールの角度・距離
            uint8_t low4 = _serial->read();                                   // ボールの角度の下位バイトを読み取る
            uint8_t high4 = _serial->read();                                  // ボールの角度の上位バイトを読み取る
            _bluegoal_deg = int16_t((uint16_t(high4) << 8) | uint16_t(low4)); // 上位バイトと下位バイトをつなげる
            uint8_t low5 = _serial->read();                                   // ボールの角度の下位バイトを読み取る
            uint8_t high5 = _serial->read();                                  // ボールの角度の上位バイトを読み取る
            _bluegoal_dis = int16_t((uint16_t(high5) << 8) | uint16_t(low5)); // 上位バイトと下位バイトをつなげる
            _bluegoal_detected = (_bluegoal_deg != -1);
        }
        else
        {
            _serial->read();
        }
    }
}
