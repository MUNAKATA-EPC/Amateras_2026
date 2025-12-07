#include "openmv.hpp"

static HardwareSerial *_serial = nullptr;
static uint32_t _baudrate = 9600U;

// コート
static bool _fieldDetected = false;
static int _fieldDeg = 0xFF;
// 青ゴール
static bool _blueGoalDetected = false;
static int _blueGoalDeg = 0xFF;
static float _blueGoalDis = 0xFF;
// 黄ゴール
static bool _yellowGoalDetected = false;
static int _yellowGoalDeg = 0xFF;
static float _yellowGoalDis = 0xFF;

static Packet_manager packet; // パケットマネージャー

bool openmvInit(HardwareSerial *serial, uint32_t baudrate)
{
    _serial = serial;
    _baudrate = baudrate;

    _serial->begin(_baudrate);

    packet.setup(0x55, 10, 0xAA); // フレームヘッダー、データサイズ、エンドヘッダーを設定

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

void openmvUpdate()
{
    // データの受け取り
    int data_count = _serial->available();
    for (int i = 0; i < data_count; i++)
    {
        packet.add(_serial->read());

        if (packet.isComplete())
        {
            uint8_t low1 = packet.get(1);                                 // コートの角度の下位バイトを読み取る
            uint8_t high1 = packet.get(2);                                // コートの角度の上位バイトを読み取る
            _fieldDeg = int16_t((uint16_t(high1) << 8) | uint16_t(low1)); // 上位バイトと下位バイトをつなげる
            _fieldDetected = (_fieldDeg != 0xFF);

            uint8_t low2 = packet.get(3);                                      // 黄色ゴールの角度の下位バイトを読み取る
            uint8_t high2 = packet.get(4);                                     // 黄色ゴールの角度の上位バイトを読み取る
            _yellowGoalDeg = int16_t((uint16_t(high2) << 8) | uint16_t(low2)); // 上位バイトと下位バイトをつなげる
            uint8_t low3 = packet.get(5);                                      // 黄色ゴールの距離の下位バイトを読み取る
            uint8_t high3 = packet.get(6);                                     // 黄色ゴールの距離の上位バイトを読み取る
            _yellowGoalDis = float((uint16_t(high3) << 8) | uint16_t(low3));   // 上位バイトと下位バイトをつなげる
            _yellowGoalDetected = (_yellowGoalDeg != 0xFF);

            uint8_t low4 = packet.get(7);                                    // 青色ゴールの角度の下位バイトを読み取る
            uint8_t high4 = packet.get(8);                                   // 青色ゴールの角度の上位バイトを読み取る
            _blueGoalDeg = int16_t((uint16_t(high4) << 8) | uint16_t(low4)); // 上位バイトと下位バイトをつなげる
            uint8_t low5 = packet.get(9);                                    // 青色ゴールの距離の下位バイトを読み取る
            uint8_t high5 = packet.get(10);                                  // 青色ゴールの距離の上位バイトを読み取る
            _blueGoalDis = float((uint16_t(high5) << 8) | uint16_t(low5));   // 上位バイトと下位バイトをつなげる
            _blueGoalDetected = (_blueGoalDeg != 0xFF);
        }
    }
}

bool fieldDetected() { return _fieldDetected; }
int fieldDeg() { return _fieldDeg; }

bool blueGoalDetected() { return _blueGoalDetected; }
int blueGoalDeg() { return _blueGoalDeg; }
float blueGoalDis() { return _blueGoalDis; }

bool yellowGoalDetected() { return _yellowGoalDetected; }
int yellowGoalDeg() { return _yellowGoalDeg; }
float yellowGoalDis() { return _yellowGoalDis; }