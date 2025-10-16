#include "ps3.hpp"

inline HardwareSerial *_serial = nullptr;
inline uint32_t _baudrate = 9600;
inline uint8_t _frameHeader = 0xAA;

// 調整
inline int _stickLeftAdjust = 0;
inline int _stickRightAdjust = 0;

// 左ステック
inline int _stickLx = 0;
inline int _stickLy = 0;
inline bool _stickLeftDetected = false;
inline int _stickLeftDeg = -1;
inline double _stickLeftDis = -1.0;
// 右ステック
inline int _stickRx = 0;
inline int _stickRy = 0;
inline bool _stickRightDetected = false;
inline int _stickRightDeg = -1;
inline double _stickRightDis = -1.0;
// ボタン（14個）
inline uint16_t buttonBitMask = 0;

bool ps3Init(HardwareSerial *serial, uint32_t baudrate, uint8_t frameHeader)
{
    _serial = serial;
    _baudrate = baudrate;
    _frameHeader = frameHeader;

    _serial->begin(_baudrate);

    return _serial->available() > 0;
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
                _stickLeftDeg = -1;
                _stickLeftDis = -1;
            }
            else
            {
                _stickLeftDetected = true;

                _stickLeftDeg = (int)round(degrees(atan2(_stickLy, _stickLx)));
                _stickLeftDeg = (_stickLeftDeg - 90 + 360) % 360; // 調整

                _stickLeftDis = sqrt(_stickLx * _stickLx + _stickLy * _stickLy); // 距離を算出
                _stickLeftDis = constrain(_stickLeftDis, 0, 128);
            }

            if (_stickRx == 0 && _stickRy == 0)
            {
                _stickRightDetected = false;
                _stickRightDeg = -1;
                _stickRightDis = -1;
            }
            else
            {
                _stickRightDetected = true;

                _stickRightDeg = (int)round(degrees(atan2(_stickRy, _stickRx)));
                _stickRightDeg = (_stickRightDeg - 90 + 360) % 360; // 調整

                _stickRightDis = sqrt(_stickRx * _stickRx + _stickRy * _stickRy); // 距離を算出
                _stickRightDis = constrain(_stickRightDis, 0, 128);
            }

            // ボタン
            uint8_t low = _serial->read();                // ボタン下位バイト
            uint8_t high = _serial->read();               // ボタン上位バイト
            buttonBitMask = (uint16_t(high) << 8) | low; // 16bit にまとめる
        }
        else
        {
            _serial->read();
        }
    }
}

bool ps3Detected(StickDataType data)
{
    switch (data)
    {
    case LEFTSTICK:
        return _stickLeftDetected;
    case RIGHTSTICK:
        return _stickRightDetected;
    }
    return 0;
}
int ps3Deg(StickDataType data)
{
    switch (data)
    {
    case LEFTSTICK:
        return _stickLeftDeg;
    case RIGHTSTICK:
        return _stickRightDeg;
    }
    return 0;
}
int ps3Dis(StickDataType data)
{
    switch (data)
    {
    case LEFTSTICK:
        return _stickLeftDis;
    case RIGHTSTICK:
        return _stickRightDis;
    }
    return 0;
}
bool ps3ButtonIsPushing(ButtonDataType type)
{
    return ((1 << (int)type) & buttonBitMask) > 0; // (int)type分だけシフトした1との論理積が0よりも大きかったらそのbitは1
}