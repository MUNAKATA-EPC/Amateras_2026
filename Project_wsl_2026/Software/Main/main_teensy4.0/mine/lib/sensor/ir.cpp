#include "ir.hpp"

//// プライベート変数・クラス・関数 ////

static HardwareSerial *_serial = nullptr;
static uint32_t _baudrate = 9600;
static bool _detected = false;
static float _deg = 0xFF;
static float _val = 0xFF;
static float _dis = 0xFF;
static float _x = 0xFF;
static float _y = 0xFF;
static float _x_speed = 0xFF;
static float _y_speed = 0xFF;

static MovementAverage _x_speed_ave(5);
static MovementAverage _y_speed_ave(5);

static PacketManager packet; // パケットマネージャー

int deg_to_dis[360] = {442, 449, 451, 449, 475, 423, 483, 418, 456, 429, 452, 417, 474, 401, 477, 460, 438, 469, 456, 421, 439, 426, 398, 476, 421, 451, 467, 441, 456, 463, 459, 469, 449, 425, 460, 471, 477, 485, 485, 479, 478, 468, 428, 460, 510, 492, 392, 418, 417, 412, 430, 432, 422, 442, 428, 425, 443, 444, 444, 442, 427, 434, 438, 410, 431, 448, 446, 440, 398, 410, 405, 412, 420, 417, 412, 431, 446, 450, 427, 418, 391, 421, 402, 399, 406, 416, 411, 398, 418, 361, 379, 426, 359, 374, 407, 401, 370, 415, 390, 369, 416, 365, 394, 427, 384, 407, 428, 429, 419, 441, 445, 419, 415, 427, 410, 391, 394, 400, 420, 413, 397, 389, 421, 414, 420, 420, 401, 419, 384, 396, 398, 401, 409, 410, 428, 410, 404, 384, 401, 415, 400, 426, 413, 395, 439, 414, 442, 410, 427, 383, 411, 390, 391, 384, 380, 398, 371, 376, 360, 397, 371, 431, 388, 439, 434, 448, 381, 390, 404, 389, 389, 378, 408, 349, 375, 382, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 386, 379, 393, 394, 389, 377, 377, 378, 300, 392, 390, 395, 350, 358, 366, 385, 352, 350, 369, 331, 342, 386, 380, 367, 407, 375, 391, 402, 363, 356, 414, 386, 405, 432, 399, 417, 371, 423, 395, 408, 394, 387, 390, 379, 343, 366, 363, 346, 348, 365, 344, 365, 333, 328, 348, 363, 319, 314, 324, 369, 366, 364, 307, 337, 300, 310, 436, 425, 430, 462, 455, 428, 379, 385, 362, 343, 357, 353, 353, 363, 354, 309, 370, 369, 336, 391, 363, 394, 370, 400, 337, 420, 351, 321, 369, 422, 380, 302, 329, 432, 330, 335, 448, 358, 342, 336, 392, 317, 313, 316, 427, 300, 370, 420, 342, 405, 323, 419, 352, 384, 399, 301, 323, 350, 300, 342, 403, 351, 417, 421, 392, 337, 423, 383, 366, 395, 300, 428, 473, 344, 418, 404, 360, 348, 407, 393, 404, 378, 428, 365, 395, 341, 398, 333, 399, 373, 395, 450, 412, 427, 422, 365, 437, 399, 453, 347, 469, 388, 443, 458, 440, 437, 440, 435, 455, 438};

//// パブリック関数 ////

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

            int index = ((int)_deg + 360) % 360;
            _dis = map(_dis, deg_to_dis[index], 1023, 300, 1023);
            _dis = constrain(_dis, 0, 1023);

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
float irDeg() { return _deg; }
float irVal() { return _val; }
float irDis() { return _dis; }

float irX() { return _x; }
float irY() { return _y; }

float irXSpeed() { return _x_speed; }
float irYSpeed() { return _y_speed; }