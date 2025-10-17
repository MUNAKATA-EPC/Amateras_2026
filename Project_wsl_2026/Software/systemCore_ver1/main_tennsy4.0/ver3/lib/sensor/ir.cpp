#include "ir.hpp"

static HardwareSerial *_serial = nullptr;
static uint32_t _baudrate = 9600;
static uint8_t _frameHeader = 0xAA;

static bool _detected = false;
static int _deg = -1;
static int _val = -1;
static double _dis = -1.0;

bool irInit(HardwareSerial *serial, uint32_t baudrate, uint8_t frameHeader)
{
    _serial = serial;
    _baudrate = baudrate;
    _frameHeader = frameHeader;

    _serial->begin(_baudrate);

    Timer timer;
    timer.reset();
    bool success = false;
    while (!success && timer.msTime() < 100)
    {
        success = _serial->available() >= 2; // 2個くらいデータが来たら成功しているとみなす
        delay(10);                           // irの通信開始待ち
    }

    return success;
}

void irUpdate()
{
    // データの受け取り
    while (_serial->available() >= 5)
    {
        while (_serial->available() > 0 && _serial->peek() != _frameHeader)
            _serial->read();

        if (_serial->available() < 5)
            break;

        if (_serial->peek() == _frameHeader)
        {
            _serial->read(); // 同期ヘッダーを捨てる

            uint8_t low1 = _serial->read();                          // ボールの角度の下位バイトを読み取る
            uint8_t high1 = _serial->read();                         // ボールの角度の上位バイトを読み取る
            _deg = int16_t((uint16_t(high1) << 8) | uint16_t(low1)); // 上位バイトと下位バイトをつなげる

            uint8_t low2 = _serial->read();                          // ボールの値の下位バイトを読み取る
            uint8_t high2 = _serial->read();                         // ボールの値の上位バイトを読み取る
            _val = int16_t((uint16_t(high2) << 8) | uint16_t(low2)); // 上位バイトと下位バイトをつなげる

            if (_deg == -1)
            {
                _detected = false;
                _val = -1;
                _dis = -1;
            }
            else
            {
                _detected = true;
                _dis = map(constrain(_val, 0, 300), 0, 300, 300, 0);
            }
        }
        else
        {
            _serial->read();
        }
    }
}

bool irDetected() { return _detected; }
int irDeg() { return _deg; }
int irVal() { return _val; }
double irDis() { return _dis; }