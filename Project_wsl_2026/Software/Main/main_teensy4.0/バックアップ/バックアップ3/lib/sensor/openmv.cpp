#include "openmv.hpp"

static HardwareSerial *_serial = nullptr;
static uint32_t _baudrate = 9600U;

// コート
static bool _field_detected = false;
static int _field_deg = 0xFF;
// 青ゴール
static bool _blue_goal_detected = false;
static int _blue_goal_deg = 0xFF;
static float _blue_goal_dis = 0xFF;
// 黄ゴール
static bool _yellow_goal_detected = false;
static int _yellow_goal_deg = 0xFF;
static float _yellow_goal_dis = 0xFF;

static PacketManager packet; // パケットマネージャー

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
        delay(10);                          // openmvの通信開始待ち
    }

    return success;
}

void openmvUpdate()
{
    // データの受け取り
    while (_serial->available() > 0)
    {
        packet.add(_serial->read());

        if (packet.isComplete())
        {
            uint8_t low1 = packet.get(1);                                  // コートの角度の下位バイトを読み取る
            uint8_t high1 = packet.get(2);                                 // コートの角度の上位バイトを読み取る
            _field_deg = int16_t((uint16_t(high1) << 8) | uint16_t(low1)); // 上位バイトと下位バイトをつなげる
            _field_detected = (_field_deg != 0xFF);

            uint8_t low2 = packet.get(3);                                        // 黄色ゴールの角度の下位バイトを読み取る
            uint8_t high2 = packet.get(4);                                       // 黄色ゴールの角度の上位バイトを読み取る
            _yellow_goal_deg = int16_t((uint16_t(high2) << 8) | uint16_t(low2)); // 上位バイトと下位バイトをつなげる
            uint8_t low3 = packet.get(5);                                        // 黄色ゴールの距離の下位バイトを読み取る
            uint8_t high3 = packet.get(6);                                       // 黄色ゴールの距離の上位バイトを読み取る
            _yellow_goal_dis = float((uint16_t(high3) << 8) | uint16_t(low3));   // 上位バイトと下位バイトをつなげる
            _yellow_goal_detected = (_yellow_goal_deg != 0xFF);

            uint8_t low4 = packet.get(7);                                      // 青色ゴールの角度の下位バイトを読み取る
            uint8_t high4 = packet.get(8);                                     // 青色ゴールの角度の上位バイトを読み取る
            _blue_goal_deg = int16_t((uint16_t(high4) << 8) | uint16_t(low4)); // 上位バイトと下位バイトをつなげる
            uint8_t low5 = packet.get(9);                                      // 青色ゴールの距離の下位バイトを読み取る
            uint8_t high5 = packet.get(10);                                    // 青色ゴールの距離の上位バイトを読み取る
            _blue_goal_dis = float((uint16_t(high5) << 8) | uint16_t(low5));   // 上位バイトと下位バイトをつなげる
            _blue_goal_detected = (_blue_goal_deg != 0xFF);
        }
    }
}

bool fieldDetected() { return _field_detected; }
int fieldDeg() { return _field_deg; }

bool blueGoalDetected() { return _blue_goal_detected; }
int blueGoalDeg() { return _blue_goal_deg; }
float blueGoalDis() { return _blue_goal_dis; }

bool yellowGoalDetected() { return _yellow_goal_detected; }
int yellowGoalDeg() { return _yellow_goal_deg; }
float yellowGoalDis() { return _yellow_goal_dis; }