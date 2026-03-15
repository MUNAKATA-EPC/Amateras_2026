#include <Arduino.h>
#include <math.h>
#include "multiplexer.hpp"

#define SENSOR_COUNT 16
#define SAMPLE_COUNT 5

Multiplexer mux;

const int pins[16] =
    {0, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1};

float ema_angle = 0;
float ema_dis = 0;

const float ANGLE_ALPHA = 0.2;
const float DIST_ALPHA = 0.25;

float cos_table[16];
float sin_table[16];

int readMedian(int pin)
{
    int v[SAMPLE_COUNT];

    for (int i = 0; i < SAMPLE_COUNT; i++)
        v[i] = mux.read(pin);

    for (int i = 0; i < SAMPLE_COUNT - 1; i++)
        for (int j = i + 1; j < SAMPLE_COUNT; j++)
            if (v[i] > v[j])
            {
                int t = v[i];
                v[i] = v[j];
                v[j] = t;
            }

    return v[SAMPLE_COUNT / 2];
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
        float rad = i * 22.5 * M_PI / 180.0;
        cos_table[i] = cos(rad);
        sin_table[i] = sin(rad);
    }
}

void loop()
{
    int val[16];
    int max_v = 0;

    for (int i = 0; i < 16; i++)
    {
        val[i] = 1023 - readMedian(pins[i]);
        if (val[i] < 0)
            val[i] = 0;

        if (val[i] > max_v)
        {
            max_v = val[i];
        }
    }

    if (max_v < 15)
    {
        sendData(0xFF, 0xFF);
        return;
    }

    float sum_x = 0;
    float sum_y = 0;

    int threshold = max_v / 4;

    for (int i = 0; i < 16; i++)
    {
        if (val[i] > threshold)
        {
            float weight = (float)val[i] * val[i];
            sum_x += weight * cos_table[i];
            sum_y += weight * sin_table[i];
        }
    }

    float current_angle = atan2(sum_y, sum_x) * 180.0 / M_PI;

    // 0〜360度に正規化
    if (current_angle < 0)
        current_angle += 360.0;

    // EMAによる角度の平滑化
    float diff = current_angle - ema_angle;

    if (diff > 180)
        diff -= 360;
    if (diff < -180)
        diff += 360;

    ema_angle += ANGLE_ALPHA * diff;

    float final_angle = ema_angle;

    while (final_angle > 180)
        final_angle -= 360;
    while (final_angle <= -180)
        final_angle += 360;

// #define EMA_DIS
#ifdef EMA_DIS
    float dis = 100000.0 / (max_v + 1.0);

    ema_dis = DIST_ALPHA * dis + (1 - DIST_ALPHA) * ema_dis;

    int dis_out = constrain((int)ema_dis, 0, 1000);
#else
    int dis_out = pow(1023 - max_v, 1.5);
#endif

    sendData(
        (int16_t)round(final_angle),
        (int16_t)dis_out);

    delay(10);
}