#include <Arduino.h>
#include <math.h>
#include "multiplexer.hpp"

Multiplexer mux;

const int pins[16] =
    {0, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1};
const double weight_gain[16] =
    {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f};

double ema_angle = 0;
const double ema_angle_plus_alpha_gain = 1.0;

double ema_dis = 0;
const double ema_dis_plus_alpha_gain = 1.0;

double cos_table[16];
double sin_table[16];

double normalizeDeg(double deg)
{
    double norm = fmod(deg + 360.0, 360.0);
    if (norm > 180)
        norm -= 360;
    return norm;
}

double diffDeg(double deg1, double deg2)
{
    double diff = (deg1 - deg2); // 差分

    double mod = fmod(diff + 360.0, 360.0);
    mod = normalizeDeg(mod);
    return mod;
}

int readMedian(int pin)
{
    const int sample_count = 1;
    int v[sample_count];

    for (int i = 0; i < sample_count; i++)
        v[i] = mux.read(pin);

    for (int i = 0; i < sample_count - 1; i++)
        for (int j = i + 1; j < sample_count; j++)
            if (v[i] > v[j])
            {
                int t = v[i];
                v[i] = v[j];
                v[j] = t;
            }

    return v[sample_count / 2];
}

void sendData(int16_t d, int16_t s)
{
    Serial1.write(0x55);
    Serial1.write((uint8_t)(d & 0xFF));
    Serial1.write((uint8_t)((d >> 8) & 0xFF));
    Serial1.write((uint8_t)(s & 0xFF));
    Serial1.write((uint8_t)((s >> 8) & 0xFF));
    Serial1.write(0xAA);
}

void setup()
{
    Serial.begin(115200);
    Serial1.begin(115200);

    mux.set_pin(1, 2, 3, 4, 0, -1);
    mux.init(20);

    for (int i = 0; i < 16; i++)
    {
        double rad = i * 22.5 * M_PI / 180.0;
        cos_table[i] = cos(rad);
        sin_table[i] = sin(rad);
    }
}

void loop()
{
    int value[16];
    int max_value = 0;
    int max_index = 0;

    for (int step = 0; step < 16; step++)
    {
        int i = (step + 8) % 16; // i=8から始まり、16で0に戻る

        value[i] = 1023 - readMedian(pins[i]);
        if (value[i] < 0)
            value[i] = 0;

        if (value[i] > max_value)
        {
            max_value = value[i];
            max_index = i;
        }

        Serial.print(String(i) + ":" + String(value[i]) + ",\t");
    }
    Serial.println();

    if (max_value < 15)
    {
        sendData(0xFF, 0xFF);
        return;
    }

    double sum_x = 0;
    double sum_y = 0;
    double sum_x_to_dis = 0;
    double sum_y_to_dis = 0;

    int threshold = max_value / 4;

    int cnt = 0;

    for (int i = 0; i < 16; i++)
    {
        if (value[i] > threshold)
        {
            double weight = (double)((int)(value[i] * value[i]) / 100 * 100) * weight_gain[i];
            sum_x += weight * cos_table[i];
            sum_y += weight * sin_table[i];

            sum_x_to_dis += (1023 - value[i]) * cos_table[i];
            sum_y_to_dis += (1023 - value[i]) * sin_table[i];
            cnt++;
        }
    }

    if (cnt > 0)
    {
        sum_x_to_dis /= cnt;
        sum_y_to_dis /= cnt;
    }

    // 現在の角度・距離
    double current_angle = static_cast<double>(atan2(sum_y, sum_x)) * 180.0 / M_PI;
    double current_dis = sqrt(sum_x_to_dis * sum_x_to_dis + sum_y_to_dis * sum_y_to_dis);

    // EMAによる角度の平滑化
    double diff_angle = diffDeg(current_angle, ema_angle);
    ema_angle += ema_angle_plus_alpha_gain * diff_angle;
    ema_angle = normalizeDeg(ema_angle);

    double diff_dis = current_dis - ema_dis;
    ema_dis += ema_dis_plus_alpha_gain * diff_dis;

    // Teensy4.0送信用データに変える
    double angle_out = ema_angle;
    double dis_out = ema_dis;

    sendData(
        (int16_t)round(angle_out),
        (int16_t)round(dis_out));

    delay(10);
}