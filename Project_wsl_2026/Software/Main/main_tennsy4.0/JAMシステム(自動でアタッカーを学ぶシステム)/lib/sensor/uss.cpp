#include "uss.hpp"

static HardwareSerial *_serial = nullptr;
static uint32_t _baudrate = 9600;

static uint16_t _left_dis = 0;
static uint16_t _right_dis = 0;

static float _left_speed = 0.0f;
static float _right_speed = 0.0f;

static MovementAverage _left_speed_ave(1);
static MovementAverage _right_speed_ave(1);

static PacketManager packet; // パケットマネージャー

bool ussInit(HardwareSerial *serial, uint32_t baudrate)
{
    _serial = serial;
    _baudrate = baudrate;

    _serial->begin(_baudrate);

    packet.setup(0x55, 4, 0xAA); // フレームヘッダー、データサイズ、エンドヘッダーを設定

    return true;
}

void ussUpdate()
{
    // データの受け取り
    bool latest_packet_received = false; // パケット受信フラグ
    uint16_t temp_right = 0;
    uint16_t temp_left = 0;

    // バッファにあるデータを全て処理する
    while (_serial->available() > 0)
    {
        packet.add(_serial->read());
        if (packet.isComplete())
        {
            // パケットが揃うたびに最新の距離情報を一時保存
            temp_right = (uint16_t(packet.get(2)) << 8) | packet.get(1);
            temp_left = (uint16_t(packet.get(4)) << 8) | packet.get(3);

            latest_packet_received = true;
            packet.reset(); // 次のパケットへ
        }
    }

    // ループ終了後、最新のデータを使って「1回だけ」速度を計算する
    if (latest_packet_received)
    {
        // 速度計算
        float raw_right_speed = (float((int32_t)temp_right - (int32_t)_right_dis)); // Δtによる除算は削除
        float raw_left_speed = -(float((int32_t)temp_left - (int32_t)_left_dis));   // Δtによる除算は削除

        // 物理的な上限でガード（異常値対策）
        raw_right_speed = constrain(raw_right_speed, -500.0f, 500.0f);
        raw_left_speed = constrain(raw_left_speed, -500.0f, 500.0f);

        // 速度計算
        _right_speed = _right_speed * 0.0f + raw_right_speed * 1.0f;
        _left_speed = _left_speed * 0.0f + raw_left_speed * 1.0f;

        _right_speed = (int)roundf(_right_speed * 100.0f) / 100.0f;
        _left_speed = (int)roundf(_left_speed * 100.0f) / 100.0f;

        _right_speed = _right_speed_ave.add(_right_speed);
        _left_speed = _left_speed_ave.add(_left_speed);

        // 距離情報の更新
        _right_dis = temp_right;
        _left_dis = temp_left;
    }
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