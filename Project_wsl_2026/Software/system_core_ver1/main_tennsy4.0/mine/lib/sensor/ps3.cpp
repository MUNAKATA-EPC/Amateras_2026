#include "ps3.hpp"

static HardwareSerial *_serial = nullptr;
static uint32_t _baudrate = 9600;

// 調整
static float _stickLeftAdjust = 0.0f;
static float _stickRightAdjust = 0.0f;

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

static Packet_manager packet; // パケットマネージャー

bool ps3Init(HardwareSerial *serial, uint32_t baudrate)
{
    _serial = serial;
    _baudrate = baudrate;

    _serial->begin(_baudrate);

    packet.setup(0x55, 8, 0xAA); // フレームヘッダー、データサイズ、エンドヘッダーを設定

    Timer timer;
    timer.reset();
    bool success = false;
    while (!success && timer.msTime() < 100)
    {
        success = _serial->available() > 0; // 1個以上データが来たら成功しているとみなす
        delay(10);                          // irの通信開始待ち
    }

    return success;
}

void ps3StickAdjust(float leftAdjust, float rightAdjust)
{
    _stickLeftAdjust = leftAdjust;
    _stickRightAdjust = rightAdjust;
}

void ps3Update()
{
    // データの受け取り
    int data_count = _serial->available();
    for (int i = 0; i < data_count; i++)
    {
        packet.add(_serial->read());

        if (packet.isComplete())
        {
            // ステックのx,y
            _stickLx = int8_t(packet.get(1));  // 左ステックX (-128~127)
            _stickLy = -int8_t(packet.get(2)); // 左ステックY
            _stickRx = int8_t(packet.get(3));  // 右ステックX
            _stickRy = -int8_t(packet.get(4)); // 右ステックY

            // 左ステックの角度・距離
            _stickLeftDis = sqrtf(_stickLx * _stickLx + _stickLy * _stickLy); // 距離を算出
            _stickLeftDis = constrain(_stickLeftDis, 0.0f, 128.0f);

            if (_stickLeftDis <= _stickLeftAdjust)
            {
                _stickLeftDetected = false;
                _stickLx = 0;
                _stickLy = 0;
                _stickLeftDeg = 0xFF;
                _stickLeftDis = 0xFF;
            }
            else
            {
                _stickLeftDetected = true;

                _stickLeftDeg = (int)round(degrees(atan2(-_stickLx, _stickLy)));
            }

            // 右ステックの角度・距離
            _stickRightDis = sqrtf(_stickRx * _stickRx + _stickRy * _stickRy); // 距離を算出
            _stickRightDis = constrain(_stickRightDis, 0.0f, 128.0f);

            if (_stickRightDis <= _stickRightAdjust)
            {
                _stickRightDetected = false;
                _stickRx = 0;
                _stickRy = 0;
                _stickRightDeg = 0xFF;
                _stickRightDis = 0xFF;
            }
            else
            {
                _stickRightDetected = true;

                _stickRightDeg = (int)round(degrees(atan2(-_stickRx, _stickRy)));
            }

            // ボタン
            uint8_t low = packet.get(5);                 // ボタン下位バイト
            uint8_t high = packet.get(6);                // ボタン上位バイト
            buttonBitMask = (uint16_t(high) << 8) | low; // 16bit にまとめる

            packet.reset();
        }
    }
}

bool ps3LeftStickDetected() { return _stickLeftDetected; }
bool ps3RightStickDetected() { return _stickRightDetected; }

int ps3LeftStickX() { return _stickLx; }
int ps3LeftStickY() { return _stickLy; }

int ps3RightStickX() { return _stickRx; }
int ps3RightStickY() { return _stickRy; }

int ps3LeftStickDeg() { return _stickLeftDeg; }
int ps3RightStickDeg() { return _stickRightDeg; }

float ps3LeftStickDis() { return _stickLeftDis; }
float ps3RightStickDis() { return _stickRightDis; }

bool ps3ButtonIsPushing(ButtonDataType type)
{
    return ((1 << (int)type) & buttonBitMask) > 0; // (int)type分だけシフトした1との論理積が0よりも大きかったらそのbitは1
}