#include "line.hpp"

static HardwareSerial *_serial = nullptr;
static uint32_t _baudrate = 9600;
static uint8_t _frameHeader = 0xAA;

static double _x = 0.0;
static double _y = 0.0;

static bool _ringDetected = false;
static bool _sideDetected = false;
static bool _sensor[19] = {false};

static bool _sideRight = false;
static bool _sideLeft = false;
static bool _sideBack = false;

static int _ringDeg = -1;
static int _sideDeg = -1;

static double _dis = -1.0;

bool lineInit(HardwareSerial *serial, uint32_t baudrate, uint8_t frameHeader)
{
    _serial = serial;
    _baudrate = baudrate;
    _frameHeader = frameHeader;

    // 初期化
    _ringDetected = _sideDetected = false;
    _ringDeg = _sideDeg = -1;
    _dis = -1;
    _sideRight = _sideLeft = _sideBack = false;
    _x = _y = 0.0;

    for (int i = 0; i < 19; i++)
        _sensor[i] = false;

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

void lineUpdate()
{
    // データの読み取り
    while (_serial->available() >= 4)
    {
        while (_serial->available() > 0 && _serial->peek() != _frameHeader)
            _serial->read();

        if (_serial->available() < 4)
            break;

        if (_serial->peek() == _frameHeader)
        {
            _serial->read(); // ヘッダー読み捨て

            uint8_t low = _serial->read();
            uint8_t middle = _serial->read();
            uint8_t high = _serial->read();

            uint32_t bit_mask = ((uint32_t)high << 16) | ((uint32_t)middle << 8) | (uint32_t)low;

            // エンジェルの検出
            _ringDetected = false;

            for (int i = 0; i < 19; i++)
            {
                _sensor[i] = (bit_mask & (1UL << i)) != 0;
                if (_sensor[i] == true && i < 16)
                    _ringDetected = true;
            }

            // 　サイドの検出
            _sideRight = _sensor[16];
            _sideLeft = _sensor[17];
            _sideBack = _sensor[18];

            _sideDetected = _sideLeft || _sideRight || _sideBack;
        }
        else
        {
            _serial->read();
        }
    }

    // 計算
    // エンジェル
    _x = 0.0; // 初期化
    _y = 0.0; // 初期化

    if (_ringDetected)
    {
        int ringDetectedCount = 0;
        for (int i = 0; i < 16; i++)
        {
            if (_sensor[i] == true)
            {
                ringDetectedCount++;
                _x += cos(radians(22.5 * i));
                _y += sin(radians(22.5 * i));
            }
        }

        if (ringDetectedCount > 0)
        {
            _x = _x * 100 / ringDetectedCount;
            _y = _y * 100 / ringDetectedCount;
            _dis = sqrt(_x * _x + _y * _y);

            _ringDeg = (int)round(degrees(atan2(_y, _x)));
            _ringDeg = (_ringDeg + 360) % 360;
        }
        else // ringDetectedCount == 0 の場合 (リングセンサーの反応がない)
        {
            _dis = -1.0;
            _ringDeg = -1;
        }
    }
    else
    {
        _dis = -1.0;
        _ringDeg = -1;
    }

    // サイドライン
    double side_x = 0.0, side_y = 0.0;

    if (_sideDetected)
    {
        side_x += (_sideRight == true) ? cos(radians(270.0)) : 0; // 前が0度としているのでx,yが反転する
        side_x += (_sideLeft == true) ? cos(radians(90.0)) : 0;   // 前が0度としているのでx,yが反転する
        side_x += (_sideBack == true) ? cos(radians(180.0)) : 0;  // 前が0度としているのでx,yが反転する

        side_y += (_sideRight == true) ? sin(radians(270.0)) : 0; // 前が0度としているのでx,yが反転する
        side_y += (_sideLeft == true) ? sin(radians(90.0)) : 0;   // 前が0度としているのでx,yが反転する
        side_y += (_sideBack == true) ? sin(radians(180.0)) : 0;  // 前が0度としているのでx,yが反転する

        _sideDeg = (int)round(degrees(atan2(side_y, side_x)));
        _sideDeg = (_sideDeg + 360) % 360;
    }
    else
    {
        _sideDeg = -1;
    }
}

bool lineRingDetected() { return _ringDetected; }
bool lineSideDetected() { return _sideDetected; }

bool lineSensorDetected(uint8_t index) { return _sensor[index]; }

bool lineSideRightDetected() { return _sideRight; }
bool lineSideLeftDetected() { return _sideLeft; }
bool lineSideBackDetected() { return _sideBack; }

int lineRingDeg() { return _ringDeg; }
int lineSideDeg() { return _sideDeg; }

double lineRingDis() { return _dis; }

double lineRingX() { return _x; }
double lineRingY() { return _y; }