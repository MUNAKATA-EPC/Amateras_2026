#include "ir.hpp"

static HardwareSerial *_serial = nullptr;
static uint32_t _baudrate = 9600;
static bool _detected = false;
static int _deg = 0xFF;
static float _val = 0xFF;
static float _dis = 0xFF;

static PacketManager packet; // パケットマネージャー

bool irInit(HardwareSerial *serial, uint32_t baudrate)
{
    _serial = serial;
    _baudrate = baudrate;

    _serial->begin(_baudrate);

    packet.setup(0x55, 4, 0xAA); // フレームヘッダー、データサイズ、エンドヘッダーを設定

    Timer timer;
    timer.reset();
    bool success = false;
    while (!success && timer.msTime() < 100UL)
    {
        success = _serial->available() > 0; // 1個以上データが来たら成功しているとみなす
        delay(10);                          // irの通信開始待ち
    }

    return success;
}

void irUpdate()
{
    // データの受け取り
    int data_count = _serial->available();
    for (int i = 0; i < data_count; i++)
    {
        packet.add(_serial->read());

        if (packet.isComplete())
        {
            uint8_t low1 = packet.get(1);  // ボールの角度の下位バイトを読み取る
            uint8_t high1 = packet.get(2); // ボールの角度の上位バイトを読み取る
            uint8_t low2 = packet.get(3);  // ボールの値の下位バイトを読み取る
            uint8_t high2 = packet.get(4); // ボールの値の上位バイトを読み取る

            _deg = int16_t((uint16_t(high1) << 8) | uint16_t(low1)); // 上位バイトと下位バイトをつなげる
            _dis = float((uint16_t(high2) << 8) | uint16_t(low2));   // 上位バイトと下位バイトをつなげる

            if (_deg == 0xFF)
            {
                _detected = false;
                _val = 0xFF;
                _dis = 0xFF;
            }
            else
            {
                _detected = true;
                _val = map(_dis, 0.0f, 1023.0f, 1023.0f, 0.0f);
                if (_val < 0.0f)
                    _val = 0.0f;
            }

            packet.reset();
        }
    }
}

bool irDetected() { return _detected; }
int irDeg() { return _deg; }
int irVal() { return _val; }
float irDis() { return _dis; }

float irX()
{
    if (!_detected)
        return 0.0f;
    float rad = radians(_deg);
    return _dis * cosf(rad);
}

float irY()
{
    if (!_detected)
        return 0.0f;
    float rad = radians(_deg);
    return _dis * sin(rad);
}