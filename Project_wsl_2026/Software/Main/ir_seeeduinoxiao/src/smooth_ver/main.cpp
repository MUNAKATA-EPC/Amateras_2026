#include <Arduino.h>
#include <stdlib.h>
#include <math.h>
#include "multiplexer.hpp"
#include "movementAverage.hpp"

// #define ATTACKER
#define DEFENDER

// 角度を -180度から180度の範囲に正規化する関数
int normalizeDeg(int deg)
{
    int norm = (deg % 360 + 360) % 360;
    if (norm > 180)
        norm -= 360;
    return norm;
}

// センサークラス (double型に変更して精度を向上)
class Sensor
{
public:
    int pin;
    bool detected;
    double value;
    double weight;
    double deg;
    double get_x() { return cos(radians(deg)); };
    double get_y() { return sin(radians(deg)); };
};

// シリアルヘッダー
const int start_header = 0x55;
const int end_header = 0xAA;

// センサー定義
Sensor IRsensor[16];

// IRセンサーのピン番号とゲイン
#ifdef ATTACKER
const int IRsensor_pin[16] = {0, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1};
const double IRsensor_weight_gain[16] = {1.05, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
                                         1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0};
#endif
#ifdef DEFENDER
const int IRsensor_pin[16] = {0, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1};
const double IRsensor_weight_gain[16] = {1.01, 0.99, 0.999, 1.0, 1.0, 1.0, 1.0, 1.0,
                                         1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 0.999, 0.99};
#endif

// マルチプレクサ
Multiplexer mux;

// 移動平均 (要素数を少し減らしてリアルタイム性を確保。ガタつきは計算ロジックで消えます)
const int ave_count = 6;
MovementAverage mux_ave[16] = {
    MovementAverage(ave_count), MovementAverage(ave_count), MovementAverage(ave_count), MovementAverage(ave_count),
    MovementAverage(ave_count), MovementAverage(ave_count), MovementAverage(ave_count), MovementAverage(ave_count),
    MovementAverage(ave_count), MovementAverage(ave_count), MovementAverage(ave_count), MovementAverage(ave_count),
    MovementAverage(ave_count), MovementAverage(ave_count), MovementAverage(ave_count), MovementAverage(ave_count)};
MovementAverage x_ave(ave_count), y_ave(ave_count);

void IRsensorDebugPrint()
{
    for (int i = 0; i < 16; i++)
    {
        Serial.print(String(IRsensor[i].pin) + ":" + String((int)IRsensor[i].weight) + ", ");
    }
}

void setup()
{
    Serial.begin(9600);
    Serial1.begin(115200);

    mux.set_pin(1, 2, 3, 4, 0, -1);
    mux.init(50); // セトルタイムを50usに短縮してリアルタイム性向上
}

void loop()
{
    int detected_count = 0;

    for (int i = 0; i < 16; i++)
    {
        IRsensor[i].pin = IRsensor_pin[i];
        // センサー値を平滑化
        IRsensor[i].value = mux_ave[i].add(mux.read(IRsensor[i].pin));
        IRsensor[i].deg = i * 22.5;

        // 反応閾値。環境に合わせて1000〜1023付近の「反応なし」の値を設定してください。
        // ここでは、値が小さいほど反応が強い（0に近づく）と仮定しています。
        double signal_strength = 1000.0 - IRsensor[i].value;

        if (signal_strength > 10.0)
        { // 990未満を検知とする
            IRsensor[i].detected = true;
            detected_count++;

            // 【重要】重みを2乗することで、最も近いセンサーのベクトルを強調し、滑らかに補間する
            IRsensor[i].weight = pow(signal_strength, 2.0) * IRsensor_weight_gain[i];
        }
        else
        {
            IRsensor[i].detected = false;
            IRsensor[i].weight = 0.0;
        }
    }

    int IRball_deg = 0xFF;
    int IRball_dis = 0xFF;

    if (detected_count != 0)
    {
        double IRball_x = 0.0, IRball_y = 0.0;

        // 【重要】7個だけでなく、反応した全センサーのベクトルを合成（段差をなくす）
        for (int i = 0; i < 16; i++)
        {
            if (IRsensor[i].detected)
            {
                IRball_x += IRsensor[i].get_x() * IRsensor[i].weight;
                IRball_y += IRsensor[i].get_y() * IRsensor[i].weight;
            }
        }

        // x, yベクトル成分を平滑化
        IRball_x = x_ave.add(IRball_x);
        IRball_y = y_ave.add(IRball_y);

        IRball_deg = (int)round(degrees(atan2(IRball_y, IRball_x)));
        IRball_deg = normalizeDeg(IRball_deg);

        // 距離は最も強いセンサーの値を基準にするなど、必要に応じて調整
        IRball_dis = (int)(IRsensor[0].value);
    }
    else
    {
        // ボールを見失った時はベクトルの平均化バッファをリセット（変な方向に引っ張られないように）
        x_ave.reset();
        y_ave.reset();
    }

    IRsensorDebugPrint();
    Serial.print("deg:" + String(IRball_deg) + "\n");

    int16_t deg_to_send = IRball_deg == 0xFF ? 0xFF : (int16_t)IRball_deg;
    int16_t dis_to_send = IRball_dis == 0xFF ? 0xFF : (int16_t)IRball_dis;

    // Teensyに送信
    Serial1.write(start_header);
    Serial1.write((uint8_t)(deg_to_send & 0xFF));        // low1
    Serial1.write((uint8_t)((deg_to_send >> 8) & 0xFF)); // high1
    Serial1.write((uint8_t)(dis_to_send & 0xFF));        // low2
    Serial1.write((uint8_t)((dis_to_send >> 8) & 0xFF)); // high2
    Serial1.write(end_header);

    delay(5); // 10msから少し短縮し、より滑らかに
}