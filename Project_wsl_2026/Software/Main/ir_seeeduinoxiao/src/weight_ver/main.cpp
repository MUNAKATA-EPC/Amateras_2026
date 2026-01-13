#include <Arduino.h>
#include "movementAverage.hpp"
#include "multiplexer.hpp"

const int start_header = 0x55; // 同期ヘッダー格納用
const int end_header = 0xAA;   // 同期ヘッダー格納用

#define IR_SENSOR_COUNT 16

Multiplexer ir_mux;
MovementAverage ir_ave[IR_SENSOR_COUNT];
MovementAverage x(3);
MovementAverage y(3);
MovementAverage dis(3);

const int IRsensor_pin[IR_SENSOR_COUNT] = {0, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1}; // 前から左回りにピン番号を指定
const float IRsensor_gain[IR_SENSOR_COUNT] = {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f};

float IRsensor_x[IR_SENSOR_COUNT]; // 前をx正方向
float IRsensor_y[IR_SENSOR_COUNT]; // 前をx正方向

int IRsensor_value[IR_SENSOR_COUNT] = {1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023}; // 平均した各IRセンサーの値を格納

void setup()
{
    Serial.begin(9600);
    Serial1.begin(115200);

    ir_mux.set_pin(1, 2, 3, 4, 0, -1);
    ir_mux.init(100);

    float diff_angle = 360.0f / IR_SENSOR_COUNT;

    for (int i = 0; i < IR_SENSOR_COUNT; i++)
    {
        ir_ave[i] = MovementAverage(3);

        IRsensor_x[i] = cosf(radians(i * diff_angle)) * 5.0f; // 座標の計算
        IRsensor_y[i] = sinf(radians(i * diff_angle)) * 5.0f; // 座標の計算

        IRsensor_value[i] = 1023;
    }
}

void loop()
{
    int min_index = 0; // 最小値のピン

    for (int i = 0; i < IR_SENSOR_COUNT; i++)
    {
        int ave_value = ir_ave[i].add(ir_mux.read(IRsensor_pin[i]) * IRsensor_gain[i]);

        IRsensor_value[i] = constrain(ave_value, 0, 1023);

        if (IRsensor_value[i] < IRsensor_value[min_index]) // 最小のピンを更新
            min_index = i;
    }

    for (int i = 0; i < IR_SENSOR_COUNT; i++)
    {
        Serial.print(String(IRsensor_value[i]) + ",");
    }
    Serial.print("->" + String(min_index) + "\n");

    bool IRball_detected = (IRsensor_value[min_index] < 1000); // ボール検出フラグ
    int16_t IRball_deg_to_send = 0xFF;
    int16_t IRball_dis_to_send = 0xFF;

    if (IRball_detected) // ボールがあるならば
    {
        const int use_count = 7; // 角度計測に使うセンサーの数

        int around_index[use_count] = {
            /*(min_index - 7 + IR_SENSOR_COUNT) % IR_SENSOR_COUNT,
            (min_index - 6 + IR_SENSOR_COUNT) % IR_SENSOR_COUNT,
            (min_index - 5 + IR_SENSOR_COUNT) % IR_SENSOR_COUNT,
            (min_index - 4 + IR_SENSOR_COUNT) % IR_SENSOR_COUNT,*/
            (min_index - 3 + IR_SENSOR_COUNT) % IR_SENSOR_COUNT,
            (min_index - 2 + IR_SENSOR_COUNT) % IR_SENSOR_COUNT,
            (min_index - 1 + IR_SENSOR_COUNT) % IR_SENSOR_COUNT,
            (min_index) % IR_SENSOR_COUNT,
            (min_index + 1) % IR_SENSOR_COUNT,
            (min_index + 2) % IR_SENSOR_COUNT,
            (min_index + 3) % IR_SENSOR_COUNT,
            /*(min_index + 4) % IR_SENSOR_COUNT,
            (min_index + 5) % IR_SENSOR_COUNT,
            (min_index + 6) % IR_SENSOR_COUNT,
            (min_index + 7) % IR_SENSOR_COUNT,*/
        };

        float weight_sum = 0.0f, weight_sum_x = 0.0f, weight_sum_y = 0.0f;

        for (int i = 0; i < use_count; i++)
        {
            int weight = 1023 - IRsensor_value[around_index[i]];

            weight_sum += weight;
            weight_sum_x += weight * IRsensor_x[around_index[i]];
            weight_sum_y += weight * IRsensor_y[around_index[i]];
        }

        float IRball_x = weight_sum_x / weight_sum;
        float IRball_y = weight_sum_y / weight_sum;

        IRball_x = x.add(IRball_x);
        IRball_y = y.add(IRball_y);

        int IRball_dis = dis.add(IRsensor_value[min_index] * 100.0f / 1023.0f);

        IRball_deg_to_send = static_cast<int16_t>(roundf(degrees(atan2f(IRball_y, IRball_x))));
        IRball_dis_to_send = static_cast<int16_t>(IRball_dis);
    }

    Serial1.write(start_header); // 同期ヘッダー

    // 2バイト送信: 下位バイト -> 上位バイトの順
    Serial1.write((uint8_t)(IRball_deg_to_send & 0xFF));        // 下位バイト (low1)
    Serial1.write((uint8_t)((IRball_deg_to_send >> 8) & 0xFF)); // 上位バイト (high1)
    // 2バイト送信: 下位バイト -> 上位バイトの順
    Serial1.write((uint8_t)(IRball_dis_to_send & 0xFF));        // 下位バイト (low2)
    Serial1.write((uint8_t)((IRball_dis_to_send >> 8) & 0xFF)); // 上位バイト (high2)

    Serial1.write(end_header); // 同期ヘッダー

    delay(10);
}
