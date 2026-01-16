#include "uss.hpp"

static HardwareSerial *_serial = nullptr;
static uint32_t _baudrate = 9600;

static uint16_t _left_dis = 0;
static uint16_t _right_dis = 0;

static float _left_speed = 0.0f;
static float _right_speed = 0.0f;

static PacketManager packet; // パケットマネージャー

bool ussInit(HardwareSerial *serial, uint32_t baudrate)
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
    }

    return success;
}

void ussUpdate()
{
    // データの受け取り
    int data_count = _serial->available();
    for (int i = 0; i < data_count; i++)
    {
        packet.add(_serial->read());

        if (packet.isComplete())
        {
            uint8_t low1 = packet.get(1);  // 右距離の下位バイトを読み取る
            uint8_t high1 = packet.get(2); // 右距離の上位バイトを読み取る
            uint8_t low2 = packet.get(3);  // 左距離の下位バイトを読み取る
            uint8_t high2 = packet.get(4); // 左距離の上位バイトを読み取る

            _right_dis = uint16_t((uint16_t(high1) << 8) | uint16_t(low1)); // 上位バイトと下位バイトをつなげる
            _left_dis = uint16_t((uint16_t(high2) << 8) | uint16_t(low2));  // 上位バイトと下位バイトをつなげる
        }
    }

    // Δt計算
    static unsigned long last_time = micros();
    unsigned long now_time = micros();
    float delta_time = float(now_time - last_time);
    last_time = now_time; // 更新

    // 速度計算
    static uint16_t old_right_dis = 0;
    if (old_right_dis == 0xFFFF)
    {
        _right_speed = 0.0f;
    }
    else
    {
        _right_speed = (_right_dis - old_right_dis) / delta_time * 1000000.0f; // cm/s
    }
    old_right_dis = _right_dis;

    static uint16_t old_left_dis = 0;
    if (old_left_dis == 0xFFFF)
    {
        _left_speed = 0.0f;
    }
    else
    {
        _left_speed = (_left_dis - old_left_dis) / delta_time * 1000000.0f; // cm/s
    }
    old_left_dis = _left_dis;
}

// 0xFFFFでないかどうか
bool ussLeftDetected() { return _left_dis != 0xFFFF; }
bool ussRightDetected() { return _right_dis != 0xFFFF; }

// cm
uint16_t ussLeftDis() { return _left_dis; }
uint16_t ussRightDis() { return _right_dis; }

// cm/s
float ussLeftSpeed() { return _left_speed; }
float ussRightSpeed() { return _right_speed; }