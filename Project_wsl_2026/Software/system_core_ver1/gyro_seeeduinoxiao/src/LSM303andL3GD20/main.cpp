#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM303_Accel.h>
#include <Adafruit_LSM303DLH_Mag.h>
#include <Adafruit_L3GD20_U.h>

Adafruit_LSM303_Accel_Unified accel = Adafruit_LSM303_Accel_Unified(54321);
Adafruit_LSM303DLH_Mag_Unified mag = Adafruit_LSM303DLH_Mag_Unified(12345);
Adafruit_L3GD20_Unified gyro = Adafruit_L3GD20_Unified(20);

// --- あなたの環境専用の補正値 ---
const float OFF_X = -15.0f;
const float OFF_Y = -50.0f;

float fusion_angle = 0.0f;
float gyro_z_offset = 0.0f;
unsigned long last_ms = 0;

void setup()
{
    Serial.begin(115200);
    Wire.begin();

    if (!accel.begin() || !mag.begin() || !gyro.begin(GYRO_RANGE_250DPS))
    {
        while (1)
            ;
    }

    // ジャイロの静止時オフセット除去
    double sum = 0;
    for (int i = 0; i < 50; i++)
    {
        sensors_event_t g;
        gyro.getEvent(&g);
        sum += g.gyro.z;
        delay(10);
    }
    gyro_z_offset = sum / 50.0;

    // 初期角度の設定
    sensors_event_t m;
    mag.getEvent(&m);
    fusion_angle = atan2(m.magnetic.y - OFF_Y, m.magnetic.x - OFF_X) * 57.2958f;
    last_ms = millis();
}

void loop()
{
    sensors_event_t a, m, g;
    accel.getEvent(&a);
    mag.getEvent(&m);
    gyro.getEvent(&g);

    float dt = (millis() - last_ms) / 1000.0f;
    last_ms = millis();

    // 1. 磁気角度の計算（補正後）
    float cal_x = m.magnetic.x - OFF_X;
    float cal_y = m.magnetic.y - OFF_Y;
    float mag_angle = atan2(cal_y, cal_x) * 57.2958f;
    if (mag_angle < 0)
        mag_angle += 360.0f;

    // 2. ジャイロ回転速度 (deg/s)
    float gz_deg = (g.gyro.z - gyro_z_offset) * 57.2958f;

    // 3. 相補フィルタによる統合
    // ALPHAが高いほどジャイロ重視（滑らか）、低いほど磁気重視（ズレに強い）
    float ALPHA = 0.96f;
    float next_angle = fusion_angle + gz_deg * dt;

    // 360度跨ぎの補正計算
    float diff = mag_angle - next_angle;
    if (diff > 180)
        diff -= 360;
    if (diff < -180)
        diff += 360;

    fusion_angle = next_angle + (1.0f - ALPHA) * diff;

    // 0-360度範囲に収める
    if (fusion_angle >= 360.0f)
        fusion_angle -= 360.0f;
    if (fusion_angle < 0.0f)
        fusion_angle += 360.0f;

    // 出力
    Serial.print("Angle:");
    Serial.println(fusion_angle, 2);

    // DAC出力 (A0ピン)
    analogWrite(A0, (int)(fusion_angle / 360.0f * 1023.0f));

    delay(10);
}