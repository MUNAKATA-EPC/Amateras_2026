#include "ir.hpp"

// #define NORMAL

#ifdef NORMAL

static HardwareSerial *_serial = nullptr;
static uint32_t _baudrate = 9600;
static bool _detected = false;
static int _deg = 0xFF;
static int _val = 0xFF;
static int _dis = 0xFF;

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
            _dis = int16_t((uint16_t(high2) << 8) | uint16_t(low2)); // 上位バイトと下位バイトをつなげる

            if (_deg == 0xFF)
            {
                _detected = false;
                _val = 0xFF;
                _dis = 0xFF;
            }
            else
            {
                _detected = true;
                _val = 100 - _dis;
                if (_val < 0)
                    _val = 0;
            }

            packet.reset();
        }
    }
}

bool irDetected() { return _detected; }
int irDeg() { return _deg; }
int irVal() { return _val; }
int irDis() { return _dis; }

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

#else

static HardwareSerial *_serial = nullptr;
static uint32_t _baudrate = 9600;

// 割り込みで書き換わるため volatile を付与（最適化による値の喪失を防ぐ）
static volatile bool _detected = false;
static volatile int _deg = 0xFF;
static volatile int _val = 0xFF;
static volatile int _dis = 0xFF;

static PacketManager packet;   // パケットマネージャー
static IntervalTimer _irTimer; // バックグラウンド処理用タイマー

// 割り込みハンドラ：FASTRUNでITCM（高速なRAM）に配置
void FASTRUN irBackgroundUpdate()
{
    if (_serial == nullptr)
        return;

    // 1回の割り込みでバッファにある全てのデータを処理しきる
    while (_serial->available() > 0)
    {
        packet.add(_serial->read());

        if (packet.isComplete())
        {
            // パケットからのデータ抽出（インライン展開されるため高速）
            uint8_t low1 = packet.get(1);
            uint8_t high1 = packet.get(2);
            uint8_t low2 = packet.get(3);
            uint8_t high2 = packet.get(4);

            // ビット演算による高速な復元
            int16_t current_deg = (int16_t)((uint16_t(high1) << 8) | uint16_t(low1));
            int16_t current_dis = (int16_t)((uint16_t(high2) << 8) | uint16_t(low2));

            if (current_deg == 0xFF)
            {
                _detected = false;
                _deg = 0xFF;
                _val = 0xFF;
                _dis = 0xFF;
            }
            else
            {
                _detected = true;
                _deg = current_deg;
                _dis = current_dis;

                // 三項演算子で分岐を簡潔にし、CPUの予測効率を向上
                int temp_val = 100 - current_dis;
                _val = (temp_val < 0) ? 0 : temp_val;
            }

            packet.reset();
        }
    }
}

bool irInit(HardwareSerial *serial, uint32_t baudrate)
{
    _serial = serial;
    _baudrate = baudrate;

    _serial->begin(_baudrate);

    packet.setup(0x55, 4, 0xAA);

    // バックグラウンドで50000マイクロ秒(50ms)ごとに監視
    _irTimer.begin(irBackgroundUpdate, 50000);

    Timer timer;
    timer.reset();
    bool success = false;
    while (!success && timer.msTime() < 100UL)
    {
        success = _serial->available() > 0;
        delay(5);
    }

    return success;
}

// 互換性のために残す タイマーが自動で行うため呼ぶ必要はない
void irUpdate() {}

bool irDetected() { return _detected; }
int irDeg() { return _deg; }
int irVal() { return _val; }
int irDis() { return _dis; }

float irX()
{
    if (!_detected)
        return 0.0f;

    float rad = (float)_deg * (PI / 180.0f);
    return (float)_dis * cosf(rad);
}

float irY()
{
    if (!_detected)
        return 0.0f;
    float rad = (float)_deg * (PI / 180.0f);
    return (float)_dis * sinf(rad);
}

#endif