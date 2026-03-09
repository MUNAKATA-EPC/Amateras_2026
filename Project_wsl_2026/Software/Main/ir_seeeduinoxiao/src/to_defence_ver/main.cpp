#include <Arduino.h>
#include <stdlib.h>
#include "multiplexer.hpp"
#include "movementAverage.hpp"

// 角度を -180度から180度の範囲に正規化する関数
int normalizeDeg(int deg)
{
    int norm = (deg % 360 + 360) % 360;
    if (norm > 180)
        norm -= 360;
    return norm;
}

// 角度の差分を -180度から180度の範囲で計算する
int diffDeg(int deg1, int deg2)
{
    int diff = (deg1 - deg2); // 差分

    // 差分が360度を超えないように調整
    int mod = (diff % 360 + 360) % 360;
    if (mod > 180)
        mod -= 360;
    return mod;
}

// センサークラス
class Sensor
{
public:
    int pin;
    float value;
    float weight;
    float deg;
    float get_x() { return cosf(radians(deg)); };
    float get_y() { return sinf(radians(deg)); };
};

// qsort()用の比較関数（降順に並べるようにする）
int sensorsCompare(const void *a, const void *b)
{
    float a_num = ((Sensor *)a)->weight;
    float b_num = ((Sensor *)b)->weight;

    if (a_num > b_num)
    {
        return -1;
    }
    else if (a_num < b_num)
    {
        return 1;
    }

    return 0;
}

// シリアルヘッダー
const int start_header = 0x55; // 同期ヘッダー格納用
const int end_header = 0xAA;   // 同期ヘッダー格納用

// センサー定義
Sensor IRsensor[16];

// IRセンサーのピン番号。前から反時計回りに指定
const int IRsensor_pin[16] = {0, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1};
const float IRsensor_weight_gain[16] = {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
                                        1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f};
// マルチプレクサ
Multiplexer mux;

// 移動平均
MovementAverage mux_ave[16] = {
    MovementAverage(10), MovementAverage(10), MovementAverage(10), MovementAverage(10),
    MovementAverage(10), MovementAverage(10), MovementAverage(10), MovementAverage(10),
    MovementAverage(10), MovementAverage(10), MovementAverage(10), MovementAverage(10),
    MovementAverage(10), MovementAverage(10), MovementAverage(10), MovementAverage(10)};

// センサーのデバッグ表示
void IRsensorDebugPrint()
{
    for (int i = 0; i < 16; i++)
    {
        Serial.print(String(IRsensor[i].pin) + ":" + String(int(IRsensor[i].weight)) + ", ");
    }
}

void setup()
{
    Serial.begin(9600);
    Serial1.begin(115200);

    // マルチプレクサのピン設定
    mux.set_pin(1, 2, 3, 4, 0, -1);
    // マルチプレクサの初期化
    mux.init(20);
}

void loop()
{
    int detected_count = 0; // いずれかのセンサーが反応したかどうか

    for (int i = 0; i < 16; i++)
    {
        IRsensor[i].pin = IRsensor_pin[i];
        IRsensor[i].value = mux_ave[i].add(mux.read(IRsensor[i].pin));
        IRsensor[i].weight = (5000.0f - IRsensor[i].value) * IRsensor_weight_gain[i];
        IRsensor[i].deg = i * 22.5f;

        if (IRsensor[i].value < 990.0f)
        {
            detected_count++;
        }
    }

    int IRball_deg = 0xFF;
    int IRball_dis = 0xFF;

    if (detected_count > 3)
    {
        qsort(IRsensor,
              sizeof(IRsensor) / sizeof(IRsensor[0]),
              sizeof(Sensor),
              sensorsCompare); // 降順に並べ替え

        int diff[2] = {-abs(diffDeg(IRsensor[0].deg, IRsensor[1].deg)), abs(diffDeg(IRsensor[0].deg, IRsensor[15].deg))};
        int add = (diff[0] + diff[1] + 0) / 3 * 0;        // 重心をとる
        IRball_deg = normalizeDeg(IRsensor[0].deg + add); // 最も重みの大きいセンサーの角度に重心を加算してボールの角度とする

        IRball_dis = (IRsensor[0].value + IRsensor[1].value + IRsensor[2].value) / 3.0f;
    }

    IRsensorDebugPrint();
    Serial.print("deg:" + String(IRball_deg) + "\n");

    int16_t deg_to_send = IRball_deg == 0xFF ? 0xFF : (int16_t)IRball_deg;
    int16_t dis_to_send = IRball_dis == 0xFF ? 0xFF : (int16_t)IRball_dis;

    // Teensyに送信
    Serial1.write(start_header); // 通信開始

    // IRball_deg (2バイト送信: 下位バイト -> 上位バイトの順)
    Serial1.write((uint8_t)(deg_to_send & 0xFF));        // 下位バイト (low1)
    Serial1.write((uint8_t)((deg_to_send >> 8) & 0xFF)); // 上位バイト (high1)
    // IRball_dis (2バイト送信: 下位バイト -> 上位バイトの順)
    Serial1.write((uint8_t)(dis_to_send & 0xFF));        // 下位バイト (low2)
    Serial1.write((uint8_t)((dis_to_send >> 8) & 0xFF)); // 上位バイト (high2)

    Serial1.write(end_header); // 通信終了

    delay(10);
}