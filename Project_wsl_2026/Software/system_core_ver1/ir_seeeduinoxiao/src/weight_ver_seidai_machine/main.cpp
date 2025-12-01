#include <Arduino.h>
#include "movement_average.hpp"
#include "multiplexer.hpp"

const int head_byte = 0xAA; // 同期ヘッダー格納用

#define IR_SENSOR_COUNT 16

Multiplexer ir_mux;
Movement_average ir_ave[IR_SENSOR_COUNT];

const int IRsensor_pin[IR_SENSOR_COUNT] = {0, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1}; // 前から左回りにピン番号を指定

float IRsensor_x[IR_SENSOR_COUNT]; // 前をx正方向
float IRsensor_y[IR_SENSOR_COUNT]; // 前をx正方向

void setup()
{
    Serial.begin(9600);
    Serial1.begin(115200);

    ir_mux.set_pin(1, 2, 3, 4, 0, -1);
    ir_mux.init(100);

    float diff_angle = 360.0f / IR_SENSOR_COUNT;

    for (int i = 0; i < IR_SENSOR_COUNT; i++)
    {
        ir_ave[i].set(3); // 3個の平均をとる

        IRsensor_x[i] = cosf(radians(i * diff_angle)); // 座標の計算
        IRsensor_y[i] = sinf(radians(i * diff_angle)); // 座標の計算
    }
}

void loop()
{
    int IRball_detected_count = 0; // ボールがあると判断したセンサーの数
    int min_value = 1023;          // センサーの最小値

    float weight_sum = 0.0f, weight_sum_x = 0.0f, weight_sum_y = 0.0f;

    for (uint8_t i = 0; i < IR_SENSOR_COUNT; i++)
    {
        ir_ave[i].add(ir_mux.read(IRsensor_pin[i]));
        int ave_value = ir_ave[i].output();

        if (ir_ave[i].cant_compute())
            ave_value = 1023;

        if (ave_value < 1000) // ボールがあるならば
        {
            IRball_detected_count++; // カウントアップ

            if (ave_value < min_value) // 最小値更新
                min_value = ave_value;

            float weight = map(ave_value, 0, 1023, 1023.0f, 0.0f); // 0 ~ 1023を1023 ~ 0に変換(重み付け用)

            weight_sum += weight;
            weight_sum_x += IRsensor_x[i] * weight;
            weight_sum_y += IRsensor_y[i] * weight;
        }
    }

    bool IRball_detected = (IRball_detected_count > 0); // ボール検出フラグ
    int16_t IRball_deg_to_send = 0xFF;
    int16_t IRball_value_to_send = 0xFF;

    if (IRball_detected) // ボールがあるならば
    {
        float IRball_x = weight_sum_x / weight_sum;
        float IRball_y = weight_sum_y / weight_sum;

        IRball_deg_to_send = static_cast<int16_t>(round(degrees(atan2(IRball_y, IRball_x))));
        IRball_value_to_send = static_cast<int16_t>(min_value);
    }

    Serial1.write(head_byte); // 同期ヘッダー
    // 2バイト送信: 下位バイト -> 上位バイトの順
    Serial1.write((uint8_t)(IRball_deg_to_send & 0xFF));        // 下位バイト (low1)
    Serial1.write((uint8_t)((IRball_deg_to_send >> 8) & 0xFF)); // 上位バイト (high1)
    // 2バイト送信: 下位バイト -> 上位バイトの順
    Serial1.write((uint8_t)(IRball_value_to_send & 0xFF));        // 下位バイト (low2)
    Serial1.write((uint8_t)((IRball_value_to_send >> 8) & 0xFF)); // 上位バイト (high2)

    delay(10);
}
