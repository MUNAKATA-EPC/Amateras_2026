#include <Arduino.h>
#include <stdlib.h>
#include "movementAverage.hpp"

#define PIN_DATA 0

#define PIN_S0 1
#define PIN_S1 2
#define PIN_S2 3

#define PIN_S3 4

/// @brief センサ反応チェック値
#define DETECTED_CHECK_VALUE 960

class TSSP58038
{
public:
    int value;
    int index;
};

TSSP58038 sensor_data[16];
TSSP58038 sensor_data_temp[16];

int16_t ball_deg;
int16_t ball_distance;

class Vector
{
private:
    double _x;
    double _y;

public:
    void set(int deg)
    {
        _x = cos(radians(deg));
        _y = sin(radians(deg));
    }

    double get_x()
    {
        return _x;
    }
    double get_y()
    {
        return _y;
    }
};

/// @brief デバッグ出力をする関数です。
void print_debug_value()
{
    for (int i = 0; i < 16; i++)
    {
        Serial.print(sensor_data[i].value);
        Serial.print(", ");
    }

    Serial.println(ball_deg);
}

/// @brief マルチプレクサーからの値を取得する関数です。
/// @param idx マルチプレクサから取得する値のインデックス。
/// @return 指定したインデックスのアナログ値。
int get_from_multiplexer(int idx)
{
    uint8_t mul_address = idx;
    uint8_t bit;

    for (uint8_t j = 0; j < 4; j++)
    {
        bit = 1 << j;

        if (bit & mul_address)
        {
            digitalWrite(j + PIN_S0, HIGH);
        }
        else
        {
            digitalWrite(j + PIN_S0, LOW);
        }
    }

    delayMicroseconds(20);

    return analogRead(PIN_DATA);
}

/// @brief センサーの値が一つでも反応しているかどうか
/// @return 反応していたらTrue、反応していなかったらFalse
bool is_any_detected()
{
    bool ret = false;
    for (int i = 0; i < 16; i++)
    {
        if (sensor_data[i].value <= DETECTED_CHECK_VALUE)
        {
            ret = true;
            break;
        }
    }

    return ret;
}

/// @brief 配列比較用関数(qsortで使用)
/// @param a 比較する値1
/// @param b 比較する値2
/// @return a=b : 0, a<b : -1, a>b : 1
int compare_int(const void *a, const void *b)
{
    int a_num = ((TSSP58038 *)a)->value;
    int b_num = ((TSSP58038 *)b)->value;

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

Movement_average value_ave[16];
Movement_average x(3);
Movement_average y(3);
void setup()
{
    Serial.begin(9600);

    pinMode(PIN_DATA, INPUT_PULLUP);
    pinMode(PIN_S0, OUTPUT);
    pinMode(PIN_S1, OUTPUT);
    pinMode(PIN_S2, OUTPUT);
    pinMode(PIN_S3, OUTPUT);

    Serial1.begin(115200);

    for (int i = 0; i < 16; i++)
        value_ave[i] = MovementAverage(3);
}

void loop()
{
    // 16ピン分のセンサーの値をマルチプレクサから取得する
    for (int i = 0; i < 16; i++)
    {
        sensor_data[i].index = i + 1;

        value_ave[i].add(get_from_multiplexer(i));
        sensor_data[i].value = value_ave[i].output();
    }

    print_debug_value();

    // ボールセンサのデバッグ用に値出力する
    if (is_any_detected())
    {
        for (int i = 0; i < 16; i++)
        {
            sensor_data_temp[i] = sensor_data[i];
        }

        qsort(sensor_data,
              sizeof(sensor_data) / sizeof(sensor_data[0]),
              sizeof(TSSP58038),
              compare_int);

        int max_detected_index = sensor_data[0].index - 1;
        int index_of_around_max_detected_sensor[7] = {
            (max_detected_index - 3 + 16) % 16,
            (max_detected_index - 2 + 16) % 16,
            (max_detected_index - 1 + 16) % 16,
            max_detected_index,
            (max_detected_index + 1) % 16,
            (max_detected_index + 2) % 16,
            (max_detected_index + 3) % 16};

        double x_of_ball_deg = 0;
        double y_of_ball_deg = 0;

        for (int i = 0; i < 7; i++)
        {
            x_of_ball_deg += cos(radians(index_of_around_max_detected_sensor[i] * -22.5)) * (1023 - sensor_data_temp[index_of_around_max_detected_sensor[i]].value);
            y_of_ball_deg += sin(radians(index_of_around_max_detected_sensor[i] * -22.5)) * (1023 - sensor_data_temp[index_of_around_max_detected_sensor[i]].value);
        }

        x.add(x_of_ball_deg * 1000000);
        y.add(y_of_ball_deg * 1000000);

        double deg_of_ball = degrees(atan2(y.output(), x.output()));

        ball_deg = (int16_t)deg_of_ball;
        ball_distance = (int16_t)(sensor_data[0].value / 1023.0 * 100.0);
    }
    else
    {
        ball_deg = 0xFF;
        ball_distance = 0xFF;
    }

    Serial1.write(0x55); // 同期ヘッダー

    // 2バイト送信: 下位バイト -> 上位バイトの順
    Serial1.write((uint8_t)(ball_deg & 0xFF));        // 下位バイト (low1)
    Serial1.write((uint8_t)((ball_deg >> 8) & 0xFF)); // 上位バイト (high1)
    // 2バイト送信: 下位バイト -> 上位バイトの順
    Serial1.write((uint8_t)(ball_distance & 0xFF));        // 下位バイト (low2)
    Serial1.write((uint8_t)((ball_distance >> 8) & 0xFF)); // 上位バイト (high2)

    Serial1.write(0xAA); // 同期ヘッダー

    delay(10);
}