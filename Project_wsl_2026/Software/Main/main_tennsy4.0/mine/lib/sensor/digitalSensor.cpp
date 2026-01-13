#include "digitalSensor.hpp"

// キャッチセンサーなどに使える万能関数
bool DigitalSensor::init(uint8_t pin)
{
    _pin = pin;

    pinMode(_pin, INPUT);

    return true;
}

bool DigitalSensor::read()
{
    return bool(digitalRead(_pin));
}

DigitalSensor catchSensor; // 共通で使えるようにしておく

// フルカラーLED用関数
bool FullColorLed::init(uint r_pin, uint8_t g_pin, uint8_t b_pin)
{
    _r_pin = r_pin;
    _g_pin = g_pin;
    _b_pin = b_pin;

    pinMode(_r_pin, OUTPUT);
    pinMode(_g_pin, OUTPUT);
    pinMode(_b_pin, OUTPUT);

    return true;
}

void FullColorLed::rgbLightUp(uint8_t r_val, uint8_t g_val, uint8_t b_val)
{
    analogWrite(_r_pin, r_val);
    analogWrite(_g_pin, g_val);
    analogWrite(_b_pin, b_val);
}

void FullColorLed::angleLightUp(int deg, uint8_t brightness)
{
    // degを0〜360の範囲に丸める (負の値にも対応)
    // 0 <= deg < 360 に正規化
    deg = (deg % 360 + 360) % 360;

    // HSV値を設定

    // H = Hue (色相: 0〜360)
    float H = (float)deg;
    // V = Value (明度: 0.0〜1.0) - brightnessから変換
    float V = (float)brightness / 255.0f;
    // S = Saturation (彩度: 1.0固定) - フルカラー設定
    const float S = 1.0f;

    float R_f, G_f, B_f; // 0.0〜1.0の最終的なRGB値

    if (brightness == 0)
    {
        // 明度が 0 の場合は、全てのLEDを消灯する
        R_f = G_f = B_f = 0.0f;
    }
    else
    {
        // Hを60で割った商と余りを求める
        float H_s = H / 60.0f;
        int i = floor(H_s); // 商 (0〜5)
        float f = H_s - i;  // 余り (0.0〜1.0)

        // HSV to RGB 変換の計算
        // 明度 V と 彩度 S=1.0 を考慮した中間値
        float p = V * (1.0f - S); // p = 0.0f
        float q = V * (1.0f - f); // q = V * (1.0f - f)
        float t = V * f;          // t = V * f

        // H のセクタ (i % 6) に基づいて R_f, G_f, B_f の値を決定
        switch (i % 6)
        {
        case 0: // 0°〜60° (赤から黄)
            R_f = V;
            G_f = t;
            B_f = p; // p=0
            break;
        case 1: // 60°〜120° (黄から緑)
            R_f = q;
            G_f = V;
            B_f = p; // p=0
            break;
        case 2:      // 120°〜180° (緑からシアン)
            R_f = p; // p=0
            G_f = V;
            B_f = t;
            break;
        case 3:      // 180°〜240° (シアンから青)
            R_f = p; // p=0
            G_f = q;
            B_f = V;
            break;
        case 4: // 240°〜300° (青からマゼンタ)
            R_f = t;
            G_f = p; // p=0
            B_f = V;
            break;
        case 5: // 300°〜360° (マゼンタから赤)
            R_f = V;
            G_f = p; // p=0
            B_f = q;
            break;
        default: // 通常発生しないが、保険として
            R_f = G_f = B_f = 0.0f;
            break;
        }
    }

    // 最終的なPWM値 (0〜255) に変換し、ピンに出力

    // R_f, G_f, B_f は V を含めて計算済みなので、255倍するだけでOK
    uint8_t r_val = (uint8_t)constrain(R_f * 255.0f, 0.0f, 255.0f);
    uint8_t g_val = (uint8_t)constrain(G_f * 255.0f, 0.0f, 255.0f);
    uint8_t b_val = (uint8_t)constrain(B_f * 255.0f, 0.0f, 255.0f);

    // フルカラーLEDを点灯
    analogWrite(_r_pin, r_val);
    analogWrite(_g_pin, g_val);
    analogWrite(_b_pin, b_val);
}

FullColorLed fullColorLed1; // 共通で使えるようにしておく