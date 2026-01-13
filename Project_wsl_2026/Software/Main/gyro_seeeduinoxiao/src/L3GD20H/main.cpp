#include <Arduino.h>
#include <Wire.h>

/* --- 設定・定数 --- */
const byte L3GD20_ADDR = 0x6A;
const float SENSITIVITY = 0.070f;
const int SAMPLE_RATE_HZ = 100;
const float DT = 1.0f / (float)SAMPLE_RATE_HZ;

// 閾値設定（ここが安定性のキモです）
const float GYRO_DEADZONE = 0.35f; // 静止時に角度が動くなら0.5まで上げてください

const int RESET_PIN = 6;
const int DAC_PIN = A0;
const int LED_PIN = 13;

#define L3GD20_CTRL1 0x20
#define L3GD20_CTRL4 0x23
#define L3GD20_Z_L 0x2C

/* --- 変数 --- */
volatile float current_angle = 0.0f;
volatile float gyro_z_offset = 0.0f;
volatile bool is_calibrating = false;

/* --- I2C高速読み取り --- */
int16_t getRawZ()
{
    Wire.beginTransmission(L3GD20_ADDR);
    Wire.write(L3GD20_Z_L | 0x80);
    if (Wire.endTransmission(false) != 0)
        return 0;
    Wire.requestFrom(L3GD20_ADDR, (byte)2);
    if (Wire.available() >= 2)
    {
        return (int16_t)(Wire.read() | (Wire.read() << 8));
    }
    return 0;
}

/* --- タイマー割り込み (TC3) --- */
void setupTimerInterrupt()
{
    GCLK->CLKCTRL.reg = GCLK_CLKCTRL_ID(GCM_TCC2_TC3) | GCLK_CLKCTRL_GEN_GCLK0 | GCLK_CLKCTRL_CLKEN;
    while (GCLK->STATUS.bit.SYNCBUSY)
        ;
    TC3->COUNT16.CTRLA.reg &= ~TC_CTRLA_ENABLE;
    while (TC3->COUNT16.STATUS.bit.SYNCBUSY)
        ;
    TC3->COUNT16.CTRLA.reg |= TC_CTRLA_MODE_COUNT16 | TC_CTRLA_WAVEGEN_MFRQ | TC_CTRLA_PRESCALER_DIV1024;
    TC3->COUNT16.CC[0].reg = 468; // 100Hz
    while (TC3->COUNT16.STATUS.bit.SYNCBUSY)
        ;
    TC3->COUNT16.INTENSET.bit.MC0 = 1;
    NVIC_EnableIRQ(TC3_IRQn);
    TC3->COUNT16.CTRLA.reg |= TC_CTRLA_ENABLE;
    while (TC3->COUNT16.STATUS.bit.SYNCBUSY)
        ;
}

void TC3_Handler()
{
    if (!is_calibrating)
    {
        int16_t raw_z = getRawZ();
        // オフセット補正
        float rate_z = (float)(raw_z - gyro_z_offset) * SENSITIVITY;

        // 【商品化の要】デッドゾーン処理
        // 静止時の微振動をカットし、ドリフトを止める
        if (abs(rate_z) < GYRO_DEADZONE)
        {
            rate_z = 0.0f;
        }

        current_angle += rate_z * DT;

        // 360度ループ処理
        if (current_angle >= 360.0f)
            current_angle -= 360.0f;
        else if (current_angle < 0.0f)
            current_angle += 360.0f;
    }
    TC3->COUNT16.INTFLAG.bit.MC0 = 1;
}

/* --- 高精度キャリブレーション --- */
void calibrate()
{
    is_calibrating = true;
    digitalWrite(LED_PIN, LOW); // 消灯＝作業中

    // センサーの安定を待つ
    delay(200);

    double sum = 0;
    const int samples = 100; // 10秒間(100Hz相当)かけてじっくり計測

    Serial.println("Calibrating... DO NOT MOVE");

    for (int i = 0; i < samples; i++)
    {
        sum += getRawZ();
        delay(10);
        if (i % 200 == 0)
            Serial.print(".");
    }

    gyro_z_offset = (float)(sum / (double)samples);
    current_angle = 0.0f;
    is_calibrating = false;

    digitalWrite(LED_PIN, HIGH); // 点灯＝準備完了
    Serial.println("\nReady.");
}

/* --- 初期設定 --- */
void setup()
{
    Serial.begin(115200);
    Wire.begin();
    Wire.setClock(400000);

    pinMode(RESET_PIN, INPUT_PULLUP);
    pinMode(LED_PIN, OUTPUT);
    analogWriteResolution(10);

    // L3GD20 起動
    Wire.beginTransmission(L3GD20_ADDR);
    Wire.write(L3GD20_CTRL1);
    Wire.write(0x0F); // Normal mode, 95Hz ODR
    Wire.endTransmission();

    Wire.beginTransmission(L3GD20_ADDR);
    Wire.write(L3GD20_CTRL4);
    Wire.write(0x20); // 2000 dps
    Wire.endTransmission();

    delay(1000); // センサーの通電安定待ち
    calibrate();
    setupTimerInterrupt();
}

/* --- メインループ --- */
void loop()
{
    // 物理リセット処理
    if (digitalRead(RESET_PIN) == LOW)
    {
        delay(100); // チャタリング除去
        if (digitalRead(RESET_PIN) == LOW)
        {
            calibrate();
            while (digitalRead(RESET_PIN) == LOW)
                ; // 離すまで待機
        }
    }

    // 出力処理
    static uint32_t last_ms = 0;
    if (millis() - last_ms >= 10)
    {
        last_ms = millis();

        // 0-360deg -> 0-1023 DAC
        uint32_t dac_val = (uint32_t)((current_angle / 360.0f) * 1023.0f);
        if (dac_val > 1023)
            dac_val = 1023;

        analogWrite(DAC_PIN, dac_val);
        Serial.println(current_angle, 2);
    }
}