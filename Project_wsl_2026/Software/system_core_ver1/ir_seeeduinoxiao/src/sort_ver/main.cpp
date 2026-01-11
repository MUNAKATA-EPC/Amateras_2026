#include <Arduino.h>
#include <stdlib.h>
#include "multiplexer.hpp"
#include "movementAverage.hpp"

// センサークラス
class Sensor
{
public:
    int value;
    float deg;
    float get_x() { return cosf(radians(deg)); };
    float get_y() { return sinf(radians(deg)); };
};

// qsort()用の比較関数（降順に並べるようにする）
int sensorsCompare(const void *a, const void *b)
{
    int a_num = ((Sensor *)a)->value;
    int b_num = ((Sensor *)b)->value;

    if (a_num < b_num)
    {
        return -1;
    }
    else if (a_num > b_num)
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
const float IRsensor_gain[16] = {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f};

// マルチプレクサ
Multiplexer mux;

// 移動平均
MovementAverage mux_ave(3), x_ave(3), y_ave(3);

// センサーのデバッグ表示
void IRsensorDebugPrint()
{
    for (int i = 0; i < 16; i++)
    {
        Serial.print(String(IRsensor[i].value) + ",");
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
    bool is_any_detected = false; // いずれかのセンサーが反応したかどうか

    for (int i = 0; i < 16; i++)
    {
        IRsensor[i].value = constrain(mux_ave.add(mux.read(IRsensor_pin[i])), 0, 1023);
        IRsensor[i].deg = i * 22.5f;

        if (IRsensor[i].value < 990)
        {
            is_any_detected = true;
        }
    }

    int IRball_deg = 0xFF;
    int IRball_dis = 0xFF;

    if (is_any_detected)
    {
        qsort(IRsensor,
              sizeof(IRsensor) / sizeof(IRsensor[0]),
              sizeof(Sensor),
              sensorsCompare); // 降順に並べ替え

        // IRsensorDebugPrint();
        // Serial.print("\n");

        const int use_count = 5; // 値の小さな7個を計算に使う

        float IRball_x = 0.0f, IRball_y = 0.0f;
        for (int i = 0; i < use_count; i++)
        {
            int weight = 1023 - IRsensor[i].value;

            IRball_x += IRsensor[i].get_x() * weight;
            IRball_y += IRsensor[i].get_y() * weight;
        }
        IRball_x = x_ave.add(IRball_x / use_count);
        IRball_y = y_ave.add(IRball_y / use_count);

        // Serial.print(String(IRball_x) + " " + String(IRball_y) + "\n");

        IRball_deg = (int)roundf(degrees(atan2f(IRball_y, IRball_x)));
        IRball_dis = IRsensor[0].value; // ※重要：1個の平均をとる
        // IRball_dis = sqrtf(IRball_x * IRball_x + IRball_y * IRball_y);
    }

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

    // delay(10);
}