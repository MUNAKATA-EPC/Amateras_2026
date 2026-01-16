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

    return true;
}

void ussUpdate()
{
    // データの受け取り
    static unsigned long last_packet_time = micros();
    int data_count = _serial->available();
    for (int i = 0; i < data_count; i++)
    {
        packet.add(_serial->read());
        if (packet.isComplete())
        {
            unsigned long now_time = micros();
            float delta_time = float(now_time - last_packet_time);
            last_packet_time = now_time;

            if (delta_time > 0)
            {
                // データのパース
                uint16_t new_right = (uint16_t(packet.get(2)) << 8) | packet.get(1);
                uint16_t new_left = (uint16_t(packet.get(4)) << 8) | packet.get(3);

                // 速度計算（LPF：前回の値を80%残す）
                float raw_right_speed = (float((int32_t)new_right - (int32_t)_right_dis)) / delta_time * 1e6f;
                float raw_left_speed = -(float((int32_t)new_left - (int32_t)_left_dis)) / delta_time * 1e6f;

                _right_speed = _right_speed * 0.3f + raw_right_speed * 0.7f;
                _left_speed = _left_speed * 0.3f + raw_left_speed * 0.7f;

                _right_dis = new_right;
                _left_dis = new_left;
            }
        }
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