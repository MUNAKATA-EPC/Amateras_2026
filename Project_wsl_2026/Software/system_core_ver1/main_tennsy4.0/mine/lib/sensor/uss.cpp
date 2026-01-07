#include "ps3.hpp"

static HardwareSerial *_serial = nullptr;
static uint32_t _baudrate = 9600;

static int _front_dis = 0;
static int _back_dis = 0;

static bool _
static int _left_dis = 0;
static int _right_dis = 0;

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
    while (!success && timer.msTime() < 100)
    {
        success = _serial->available() > 0; // 1個以上データが来たら成功しているとみなす
        delay(10);                          // ussの通信開始待ち
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

            _right_dis = int16_t((uint16_t(high1) << 8) | uint16_t(low1)); // 上位バイトと下位バイトをつなげる
            _left_dis = int16_t((uint16_t(high2) << 8) | uint16_t(low2));  // 上位バイトと下位バイトをつなげる
        }
    }
}

int ussFrontDis() { return _front_dis; }
int ussBackDis() { return _back_dis; }

int ussLeftDis() { return _left_dis; }
int ussRightDis() { return _right_dis; }