#include "IR.hpp"

IR::IR(HardwareSerial &serial, uint32_t baudrate, uint8_t frameHeader) : _serial(serial), _baudrate(baudrate), _frameHeader(frameHeader)
{
    _serial.begin(_baudrate);
}

void IR::update()
{
    // データの受け取り
    while (_serial.available() >= 5)
    {
        while (_serial.available() > 0 && _serial.peek() != _frameHeader)
            _serial.read();

        if (_serial.available() < 5)
            break;

        if (_serial.peek() == _frameHeader)
        {
            _serial.read(); // 同期ヘッダーを捨てる

            uint8_t low1 = _serial.read();                          // ボールの角度の下位バイトを読み取る
            uint8_t high1 = _serial.read();                         // ボールの角度の上位バイトを読み取る
            deg = int((uint16_t(high1) << 8) | uint16_t(low1)); // 上位バイトと下位バイトをつなげる

            uint8_t low2 = _serial.read();                          // ボールの値の下位バイトを読み取る
            uint8_t high2 = _serial.read();                         // ボールの値の上位バイトを読み取る
            val = int((uint16_t(high2) << 8) | uint16_t(low2)); // 上位バイトと下位バイトをつなげる

            if (deg == -1)
            {
                detected = false;
                val = -1;
                dis = -1;
            }
            else
            {
                detected = true;
                dis = map(constrain(val, 0, 300), 0, 300, 300, 0);
            }
        }
        else
        {
            _serial.read();
        }
    }
}
