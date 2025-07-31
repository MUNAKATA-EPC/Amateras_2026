#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include "USE_m5pico.h"

#define LED_OUTPUTPIN 23 // D23をoutputとして使用
#define LEDs 24          // 24個のLEDを使用
#define BRIGHTMAXNUM 20  // 明るさの上限

int r, g, b; // 赤・緑・青
void getRGBnum(float RGBedge, int &r, int &g, int &b);
void SamefulLight(int RGBEdge);
void ColorfulLight(int RedEdge);

Adafruit_NeoPixel led(LEDs, LED_OUTPUTPIN, NEO_GRB + NEO_KHZ800);

const int m5pico_TalkStart_ms = 4500;

void setup()
{
    led.begin();                     // 初期化
    led.setBrightness(BRIGHTMAXNUM); // 明るさ（最大255）
    led.clear();                     // 全部消す
    led.show();                      // 表示

    // M5-picoとのシリアルの設定
    M5PICObegin(9600);    // D6をRX,D7をTXとしている。通信速度は9600でM5-picoと通信開始
    Stick_Adjust(10, 10); // AnalogStickの-1とする範囲の調整。X・Yどちらも±5は-1としている。
}

void loop()
{
    M5PICOget(); // M5-picoからの値を更新

    ColorfulLight((millis() / 5) % 360); // 角度iを赤色としてカラフルにする

    led.show(); // 実際に表示する
    delay(50);
    led.clear(); // 全部消す
}

void getRGBnum(float RGBedge, int &r, int &g, int &b)
{
    float s = 1.0; // 彩度（最大）
    float v = 1.0; // 明度（最大）

    float c = v * s;
    float x = c * (1 - fabs(fmod(RGBedge / 60.0, 2) - 1));
    float m = v - c;

    float r1, g1, b1;

    if (RGBedge < 60)
    {
        r1 = c;
        g1 = x;
        b1 = 0;
    }
    else if (RGBedge < 120)
    {
        r1 = x;
        g1 = c;
        b1 = 0;
    }
    else if (RGBedge < 180)
    {
        r1 = 0;
        g1 = c;
        b1 = x;
    }
    else if (RGBedge < 240)
    {
        r1 = 0;
        g1 = x;
        b1 = c;
    }
    else if (RGBedge < 300)
    {
        r1 = x;
        g1 = 0;
        b1 = c;
    }
    else
    {
        r1 = c;
        g1 = 0;
        b1 = x;
    }

    r = (r1 + m) * 255;
    g = (g1 + m) * 255;
    b = (b1 + m) * 255;
}

void SamefulLight(int RGBEdge)
{
    RGBEdge = abs(RGBEdge - 360);
    for (int i = 0; i < LEDs; i++)
    {
        getRGBnum(RGBEdge, r, g, b);
        led.setPixelColor(i, led.Color(r, g, b)); // i番LEDを光らせる
    }
}

void ColorfulLight(int RedEdge)
{
    RedEdge = abs(RedEdge - 360);
    for (int i = 0; i < LEDs; i++)
    {
        getRGBnum((RedEdge + 360 / LEDs * i) % 360, r, g, b);
        led.setPixelColor(i, led.Color(r, g, b)); // i番LEDを光らせる
    }
}
