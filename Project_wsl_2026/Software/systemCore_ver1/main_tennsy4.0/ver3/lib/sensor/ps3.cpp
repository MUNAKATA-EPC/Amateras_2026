#include "ps3.hpp"

static HardwareSerial *_serial = nullptr;
static uint32_t _baudrate = 9600;
static uint8_t _frameHeader = 0xAA;

// 調整
static int _stickLeftAdjust = 0;
static int _stickRightAdjust = 0;

// 左ステック
static int _stickLx = 0;
static int _stickLy = 0;
static bool _stickLeftDetected = false;
static int _stickLeftDeg = 0xFF;
static float _stickLeftDis = 0xFF;
// 右ステック
static int _stickRx = 0;
static int _stickRy = 0;
static bool _stickRightDetected = false;
static int _stickRightDeg = 0xFF;
static float _stickRightDis = 0xFF;
// ボタン（14個）
static uint16_t buttonBitMask = 0;

bool ps3Init(HardwareSerial *serial, uint32_t baudrate, uint8_t frameHeader)
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
        success = _serial->available() > 0;  // 1個以上データが来たら成功しているとみなす
        delay(10);                           // irの通信開始待ち
    }

    return success;
}

void ps3StickAdjust(int leftAdjust, int rightAdjust)
{
    _stickLeftAdjust = leftAdjust;
    _stickRightAdjust = rightAdjust;
}

void ps3Update()
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
            _stickLx = (int8_t)_serial->read(); // 左ステックX (-128~127)
            if (abs(_stickLx) < abs(_stickLeftAdjust))
                _stickLx = 0;
            _stickLy = -(int8_t)(_serial->read()); // 左ステックY
            if (abs(_stickLy) < abs(_stickLeftAdjust))
                _stickLy = 0;
            _stickRx = (int8_t)_serial->read(); // 右ステックX
            if (abs(_stickRx) < abs(_stickRightAdjust))
                _stickRx = 0;
            _stickRy = -(int8_t)(_serial->read()); // 右ステックY
            if (abs(_stickRy) < abs(_stickRightAdjust))
                _stickRy = 0;

            // ステックの角度・距離
            if (_stickLx == 0 && _stickLy == 0)
            {
                _stickLeftDetected = false;
                _stickLeftDeg = 0xFF;
                _stickLeftDis = 0xFF;
            }
            else
            {
                _stickLeftDetected = true;

                _stickLeftDeg = (int)round(degrees(atan2(_stickLy, _stickLx)));
                _stickLeftDeg = (_stickLeftDeg - 90 + 360) % 360; // 調整
                _stickLeftDeg = _stickLeftDeg < 180 ? _stickLeftDeg : 360 - _stickLeftDeg;

                _stickLeftDis = sqrtf(_stickLx * _stickLx + _stickLy * _stickLy); // 距離を算出
                _stickLeftDis = constrain(_stickLeftDis, 0.0f, 128.0f);
            }

            if (_stickRx == 0 && _stickRy == 0)
            {
                _stickRightDetected = false;
                _stickRightDeg = 0xFF;
                _stickRightDis = 0xFF;
            }
            else
            {
                _stickRightDetected = true;

                _stickRightDeg = (int)round(degrees(atan2(_stickRy, _stickRx)));
                _stickRightDeg = (_stickRightDeg - 90 + 360) % 360; // 調整
                _stickRightDeg = _stickRightDeg < 180 ? _stickRightDeg : 360 - _stickRightDeg;

                _stickRightDis = sqrtf(_stickRx * _stickRx + _stickRy * _stickRy); // 距離を算出
                _stickRightDis = constrain(_stickRightDis, 0.0f, 128.0f);
            }

            // ボタン
            uint8_t low = _serial->read();               // ボタン下位バイト
            uint8_t high = _serial->read();              // ボタン上位バイト
            buttonBitMask = (uint16_t(high) << 8) | low; // 16bit にまとめる
        }
        else
        {
            _serial->read();
        }
    }
}

bool ps3LeftStickDetected() { return _stickLeftDetected; }
bool ps3RightStickDetected() { return _stickRightDetected; }

int ps3LeftStickDeg() { return _stickLeftDeg; }
int ps3RightStickDeg() { return _stickRightDeg; }

float ps3LeftStickDis() { return _stickLeftDis; }
float ps3RightStickDis() { return _stickRightDis; }

bool ps3ButtonIsPushing(ButtonDataType type)
{
    return ((1 << (int)type) & buttonBitMask) > 0; // (int)type分だけシフトした1との論理積が0よりも大きかったらそのbitは1
}