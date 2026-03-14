#include "ir.hpp"

static HardwareSerial *_serial = nullptr;
static uint32_t _baudrate = 9600;
static bool _detected = false;
static int _deg = 0xFF;
static int _val = 0xFF;
static int _dis = 0xFF;
static float _x = 0xFF;
static float _y = 0xFF;
static float _x_speed = 0xFF;
static float _y_speed = 0xFF;

static MovementAverage _x_speed_ave(5);
static MovementAverage _y_speed_ave(5);

static PacketManager packet; // パケットマネージャー

bool irInit(HardwareSerial *serial, uint32_t baudrate)
{
    _serial = serial;
    _baudrate = baudrate;

    _serial->begin(_baudrate);

    packet.setup(0x55, 4, 0xAA); // フレームヘッダー、データサイズ、エンドヘッダーを設定

    return true;
}

void irUpdate()
{
    bool latest_packet_received = false; // 今回のループでパケットが完成したかフラグ

    // whileループ内ではデータの受信とパースのみを行い、速度計算は最後に行う
    while (_serial->available() > 0)
    {
        packet.add(_serial->read());

        if (packet.isComplete())
        {
            // パケットが完成するたびにデータを読み出す（30バイト分あれば数回更新される）
            uint8_t low1 = packet.get(1);
            uint8_t high1 = packet.get(2);
            uint8_t low2 = packet.get(3);
            uint8_t high2 = packet.get(4);

            _deg = int16_t((uint16_t(high1) << 8) | uint16_t(low1));
            _dis = int16_t((uint16_t(high2) << 8) | uint16_t(low2));

            latest_packet_received = true; // パケットが1つ以上完成したことを記録
            packet.reset();                // 次のパケットへ
        }
    }

    // ループ終了後、最新のデータを使って「1回だけ」速度を計算する
    if (latest_packet_received)
    {
        if (_deg == 0xFF)
        {
            _detected = false;
            _val = 0xFF;
            _dis = 0xFF;
            _x = 0xFF;
            _y = 0xFF;
            _x_speed = 0xFF;
            _y_speed = 0xFF;
        }
        else
        {
            float new_x = _dis * cosf(radians(_deg));
            float new_y = _dis * sinf(radians(_deg));

            if (_detected)
            {
                // 分母(delta_time)が前回の実行時からの差分になるため、安定する
                float raw_x_speed = (new_x - _x); // Δtによる除算は削除
                float raw_y_speed = (new_y - _y); // Δtによる除算は削除

                raw_x_speed = constrain(raw_x_speed, -500.0f, 500.0f);
                raw_y_speed = constrain(raw_y_speed, -500.0f, 500.0f);

                _x_speed = (_x_speed * 0.3f) + (raw_x_speed * 0.7f);
                _y_speed = (_y_speed * 0.3f) + (raw_y_speed * 0.7f);

                /*_x_speed = (int)roundf(_x_speed * 100.0f) / 100.0f;
                _y_speed = (int)roundf(_y_speed * 100.0f) / 100.0f;*/

                _x_speed = _x_speed_ave.add(_x_speed);
                _y_speed = _y_speed_ave.add(_y_speed);
            }
            else
            {
                _x_speed = 0.0f;
                _y_speed = 0.0f;
            }

            _x = new_x;
            _y = new_y;
            _detected = true;
            _val = 100 - _dis;
            if (_val < 0)
                _val = 0;
        }
    }
}
bool irDetected() { return _detected; }
int irDeg() { return _deg; }
int irVal() { return _val; }
int irDis() { return _dis; }

float irX() { return _x; }
float irY() { return _y; }

float irXSpeed() { return _x_speed; }
float irYSpeed() { return _y_speed; }