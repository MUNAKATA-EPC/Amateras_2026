#include "ps3.hpp"

static HardwareSerial *_serial = nullptr;
static uint32_t _baudrate = 9600;

// 調整
static float _stick_left_adjust = 0.0f;
static float _stick_right_adjust = 0.0f;

// 左ステック
static int _stick_lx = 0;
static int _stick_ly = 0;
static bool _stick_left_detected = false;
static int _stick_left_deg = 0xFF;
static float _stick_left_dis = 0xFF;
// 右ステック
static int _stick_rx = 0;
static int _stick_ry = 0;
static bool _stick_right_detected = false;
static int _stick_right_deg = 0xFF;
static float _stick_right_dis = 0xFF;
// ボタン（14個）
static uint16_t _button_bit_mask = 0;

static PacketManager packet; // パケットマネージャー

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
    _stick_left_adjust = leftAdjust;
    _stick_right_adjust = rightAdjust;
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
            _stick_lx = int8_t(packet.get(1));  // 左ステックX (-128~127)
            _stick_ly = -int8_t(packet.get(2)); // 左ステックY
            _stick_rx = int8_t(packet.get(3));  // 右ステックX
            _stick_ry = -int8_t(packet.get(4)); // 右ステックY

            // 左ステックの角度・距離
            _stick_left_dis = sqrtf(_stick_lx * _stick_lx + _stick_ly * _stick_ly); // 距離を算出
            _stick_left_dis = constrain(_stick_left_dis, 0.0f, 128.0f);

            if (_stick_left_dis <= _stick_left_adjust)
            {
                _stick_left_detected = false;
                _stick_lx = 0;
                _stick_ly = 0;
                _stick_left_deg = 0xFF;
                _stick_left_dis = 0xFF;
            }
            else
            {
                _stick_left_detected = true;

                _stick_left_deg = (int)round(degrees(atan2(-_stick_lx, _stick_ly)));
            }

            // 右ステックの角度・距離
            _stick_right_dis = sqrtf(_stick_rx * _stick_rx + _stick_ry * _stick_ry); // 距離を算出
            _stick_right_dis = constrain(_stick_right_dis, 0.0f, 128.0f);

            if (_stick_right_dis <= _stick_right_adjust)
            {
                _stick_right_detected = false;
                _stick_rx = 0;
                _stick_ry = 0;
                _stick_right_deg = 0xFF;
                _stick_right_dis = 0xFF;
            }
            else
            {
                _stick_right_detected = true;

                _stick_right_deg = (int)round(degrees(atan2(-_stick_rx, _stick_ry)));
            }

            // ボタン
            uint8_t low = packet.get(5);                 // ボタン下位バイト
            uint8_t high = packet.get(6);                // ボタン上位バイト
            _button_bit_mask = (uint16_t(high) << 8) | low; // 16bit にまとめる

            packet.reset();
        }
    }
}

bool ps3LeftStickDetected() { return _stick_left_detected; }
bool ps3RightStickDetected() { return _stick_right_detected; }

int ps3LeftStickX() { return _stick_lx; }
int ps3LeftStickY() { return _stick_ly; }

int ps3RightStickX() { return _stick_rx; }
int ps3RightStickY() { return _stick_ry; }

int ps3LeftStickDeg() { return _stick_left_deg; }
int ps3RightStickDeg() { return _stick_right_deg; }

float ps3LeftStickDis() { return _stick_left_dis; }
float ps3RightStickDis() { return _stick_right_dis; }

bool ps3ButtonIsPushing(ButtonDataType type)
{
    return ((1 << (int)type) & _button_bit_mask) > 0; // (int)type分だけシフトした1との論理積が0よりも大きかったらそのbitは1
}