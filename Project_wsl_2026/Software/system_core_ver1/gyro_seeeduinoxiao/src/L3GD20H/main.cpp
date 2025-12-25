#include <Arduino.h>
#include <Wire.h>
#include <MsTimer2.h> // タイマー割り込み用ライブラリ

/* ----------------------------------------------------------------
 * 設定・定数
 * ---------------------------------------------------------------- */
const byte L3GD20_ADDR = 0x6A;                   // SA0=GND: 0x6A, VDD_IO: 0x6B
const float L3GD20_SENSITIVITY_2000DPS = 0.070f; // 2000dps設定時の感度(deg/LSB)
const int SAMPLE_RATE_MS = 10;                   // サンプリング間隔 (10ms = 100Hz)
const float DT = (float)SAMPLE_RATE_MS / 1000.0f;

// レジスタアドレス
#define L3GD20_CTRL1 0x20
#define L3GD20_CTRL4 0x23
#define L3GD20_Z_L 0x2C
#define L3GD20_Z_H 0x2D

/* ----------------------------------------------------------------
 * 変数
 * ---------------------------------------------------------------- */
volatile float current_angle = 0.0f; // 現在の角度(deg)
volatile float gyro_z_offset = 0.0f; // キャリブレーション値
bool is_calibrating = false;

/* ----------------------------------------------------------------
 * I2C通信関数
 * ---------------------------------------------------------------- */
void writeReg(byte reg, byte val)
{
    Wire.beginTransmission(L3GD20_ADDR);
    Wire.write(reg);
    Wire.write(val);
    Wire.endTransmission();
}

int16_t readZRaw()
{
    Wire.beginTransmission(L3GD20_ADDR);
    Wire.write(L3GD20_Z_L | 0x80); // 複数バイト読み込み用フラグ（L3GD20仕様）
    Wire.endTransmission();
    Wire.requestFrom(L3GD20_ADDR, (byte)2);

    if (Wire.available() >= 2)
    {
        uint8_t l = Wire.read();
        uint8_t h = Wire.read();
        return (int16_t)((h << 8) | l);
    }
    return 0;
}

/* ----------------------------------------------------------------
 * コアロジック：タイマー割り込みによる積分処理
 * ---------------------------------------------------------------- */
void updateGyro()
{
    if (is_calibrating)
        return;

    int16_t raw_z = readZRaw();

    // オフセット除去と感度補正
    // 単位: (Raw - Offset) * Sensitivity = deg/sec
    float rate_z = (float)(raw_z - gyro_z_offset) * L3GD20_SENSITIVITY_2000DPS;

    // ノイズ除去（不感帯：デッドゾーンの設定）
    // 静止時にわずかに動く値をカットすることでドリフトを抑える
    if (abs(rate_z) < 0.15f)
        rate_z = 0.0f;

    // 積分：角度 = 角度 + (角速度 * 時間)
    current_angle += rate_z * DT;
}

/* ----------------------------------------------------------------
 * 高精度キャリブレーション
 * ---------------------------------------------------------------- */
void calibrateGyro()
{
    is_calibrating = true;
    long sum = 0;
    const int samples = 500; // 5秒間 (10ms * 500)

    Serial.println("Calibrating... DO NOT MOVE");
    for (int i = 0; i < samples; i++)
    {
        sum += readZRaw();
        delay(SAMPLE_RATE_MS);
        if (i % 100 == 0)
            Serial.print(".");
    }
    gyro_z_offset = (float)sum / (float)samples;
    current_angle = 0.0f; // 角度リセット
    is_calibrating = false;
    Serial.println("\nCalibration Done.");
}

/* ----------------------------------------------------------------
 * メイン処理
 * ---------------------------------------------------------------- */
void setup()
{
    Serial.begin(115200); // 高速通信推奨
    Wire.begin();
    Wire.setClock(400000); // I2Cを高速モード(400kHz)に設定

    // L3GD20初期化
    // 0x0F = Normal mode, All axes enabled, 95Hz ODR
    writeReg(L3GD20_CTRL1, 0x0F);
    // 0x20 = Full scale 2000 dps
    writeReg(L3GD20_CTRL4, 0x20);

    delay(200);
    calibrateGyro();

    // タイマー割り込み開始 (10ms周期でupdateGyroを実行)
    MsTimer2::set(SAMPLE_RATE_MS, updateGyro);
    MsTimer2::start();
}

void loop()
{
    // メインループでは表示のみを行い、計算を邪魔しない
    static unsigned long last_print = 0;
    if (millis() - last_print > 100)
    { // 100msごとに表示
        Serial.print("Angle_Z: ");
        Serial.println(current_angle, 2); // 小数点第2位まで表示
        last_print = millis();
    }

    // リセットボタン（外部スイッチ等）の処理が必要な場合はここに追加
}