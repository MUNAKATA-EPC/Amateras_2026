#include "openmv.hpp"

inline HardwareSerial *_serial = nullptr;
inline uint32_t _baudrate = 9600;
inline uint8_t _frameHeader = 0xAA;

// コート
inline bool _fieldDetected = false;
inline int _fieldDeg = -1;
// 青ゴール
inline bool _blueGoalDetected = false;
inline int _blueGoalDeg = -1;
inline double _blueGoalDis = -1;
// 黄ゴール
inline bool _yellowGoalDetected = false;
inline int _yellowGoalDeg = -1;
inline double _yellowGoalDis = -1;

bool openmvInit(HardwareSerial *serial, uint32_t baudrate, uint8_t frameHeader)
{
    _serial = serial;
    _baudrate = baudrate;
    _frameHeader = frameHeader;

    _serial->begin(_baudrate);

    return _serial->available() > 0;
}

void openmvUpdate()
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
            _fieldDeg = int16_t((uint16_t(high1) << 8) | uint16_t(low1)); // 上位バイトと下位バイトをつなげる
            _fieldDetected = (_fieldDeg != -1);

            // 黄色ゴールの角度・距離
            uint8_t low2 = _serial->read();                                     // ボールの角度の下位バイトを読み取る
            uint8_t high2 = _serial->read();                                    // ボールの角度の上位バイトを読み取る
            _yellowGoalDeg = int16_t((uint16_t(high2) << 8) | uint16_t(low2)); // 上位バイトと下位バイトをつなげる
            uint8_t low3 = _serial->read();                                     // ボールの角度の下位バイトを読み取る
            uint8_t high3 = _serial->read();                                    // ボールの角度の上位バイトを読み取る
            _yellowGoalDis = int16_t((uint16_t(high3) << 8) | uint16_t(low3)); // 上位バイトと下位バイトをつなげる
            _yellowGoalDetected = (_yellowGoalDeg != -1);

            // 青色ゴールの角度・距離
            uint8_t low4 = _serial->read();                                   // ボールの角度の下位バイトを読み取る
            uint8_t high4 = _serial->read();                                  // ボールの角度の上位バイトを読み取る
            _blueGoalDeg = int16_t((uint16_t(high4) << 8) | uint16_t(low4)); // 上位バイトと下位バイトをつなげる
            uint8_t low5 = _serial->read();                                   // ボールの角度の下位バイトを読み取る
            uint8_t high5 = _serial->read();                                  // ボールの角度の上位バイトを読み取る
            _blueGoalDis = int16_t((uint16_t(high5) << 8) | uint16_t(low5)); // 上位バイトと下位バイトをつなげる
            _blueGoalDetected = (_blueGoalDeg != -1);
        }
        else
        {
            _serial->read();
        }
    }
}

bool m5stampDetected(CamDataType data)
{
    switch (data)
    {
    case FIELD:
        return _fieldDetected;
    case YELLOWGOAL:
        return _yellowGoalDetected;
    case BLUEGOAL:
        return _blueGoalDetected;
    }
    return 0;
}
int m5stampDeg(CamDataType data)
{
    switch (data)
    {
    case FIELD:
        return _fieldDeg;
    case YELLOWGOAL:
        return _yellowGoalDeg;
    case BLUEGOAL:
        return _blueGoalDeg;
    }
    return 0;
}
int m5stampDis(CamDataType data)
{
    switch (data)
    {
    case FIELD:
        return 0;
    case YELLOWGOAL:
        return _yellowGoalDis;
    case BLUEGOAL:
        return _blueGoalDis;
    }
    return 0;
}
