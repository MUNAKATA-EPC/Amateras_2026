#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include "USE_NeoPixel.h"

int LEDs;          // ledの数を格納
int OutputPin;     // OutputPinを格納
int LightMaxValue; // LEDの最大光量を格納
int r, g, b;       // 色相環のRGBを格納する変数

Adafruit_NeoPixel led = Adafruit_NeoPixel(1, 1, NEO_GRB + NEO_KHZ800);

void NeoPixel_begin(int a, int b, int c)
{
    // それぞれの変数に代入する
    LEDs = a;
    OutputPin = b;
    LightMaxValue = c;

    // NeoPixelを初期化する
    led.updateLength(LEDs);           // LED数更新
    led.setPin(OutputPin);            // 出力ピン更新
    led.begin();                      // 初期化
    led.setBrightness(LightMaxValue); // 明るさ（最大255）
    led.clear();                      // 全部消す
    led.show();                       // 表示
}

void NeoPixel_Show()
{
    // NeoPixelに表示する
    led.show();
}

void NeoPixel_Clear()
{
    // NeoPixelをクリアにする
    led.clear();
}

void getRGBnum(int RGBedge, int &r, int &g, int &b)
{
    RGBedge %= 360; // 0~360に納める

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

void One_NeoLight(int LEDpin, int RGBedge, int PerBright)
{
    double BrightNum = LightMaxValue * PerBright / 100;
    led.setBrightness(int(BrightNum)); // 明るさ設定

    LEDpin = (LEDpin + LEDs) % LEDs; // 0~LEDs-1に収める

    getRGBnum(RGBedge, r, g, b);
    led.setPixelColor(LEDpin, led.Color(r, g, b)); // i番LEDを光らせる
}

void Mapping_NeoLight(int fromLEDpin, int toLEDpin, int LightLEDs, int minRGBedge, int maxRGBedge, int PerBright)
{
    fromLEDpin = (fromLEDpin + LEDs) % LEDs;
    toLEDpin = (toLEDpin + LEDs) % LEDs;

    int RGBcount = 0;
    int RGBadd = int((maxRGBedge - minRGBedge) / (LightLEDs - 1));

    // from → to の方向に LightLEDs 個並べる（常に時計回り）
    for (int i = 0; i < LightLEDs; i++)
    {
        int LEDpin = (fromLEDpin + i) % LEDs;
        One_NeoLight(LEDpin, minRGBedge + RGBadd * RGBcount, PerBright);
        RGBcount++;
    }
}

unsigned long OldTime_Turn = millis();
double LEDPoint_Turn = 0.0000;

void SpeedTurn_NeoLight(double Speed, int PerBright)
{
    unsigned long NowTime_Turn = millis();
    LEDPoint_Turn += (NowTime_Turn - OldTime_Turn) * Speed / 1000.0;
    int LEDpinNum = round(LEDPoint_Turn);
    LEDpinNum %= LEDs;
    Mapping_NeoLight(LEDpinNum, LEDpinNum - 1, 24, 0, 345, 100);
    OldTime_Turn = millis();
}

unsigned long OldTime_Meter = millis();
int LEDLightPin_Meter = 0;
bool MeterMax = 0;

void Meter_NeoLight(double WhileTime, int PerBright)
{
    unsigned long NewTime_Meter = millis() - OldTime_Meter;
    if (NewTime_Meter >= WhileTime)
    {
        LEDLightPin_Meter++;
        OldTime_Meter = millis();
    }

    if (LEDLightPin_Meter > LEDs - 1)
    {
        MeterMax = 1;
    }
    else
    {
        MeterMax = 0;
        Mapping_NeoLight(0, LEDLightPin_Meter, LEDLightPin_Meter + 1, 0, 0, 100);
    }
}

void Meter_NeoLight_Clear()
{
    MeterMax = 0;
    LEDLightPin_Meter = 0;
    OldTime_Meter = millis();
}

void LEDedge_NeoLight(int LEDedge, int PlusMiusLEDs, int minRGBedge, int maxRGBedge, int PerBright)
{
    if (LEDedge != -1)
    {
        int LEDmiddlePin = map(LEDedge, 0, 360, 0, LEDs);
        LEDmiddlePin %= LEDs;
        int LEDminPin = round(LEDmiddlePin - PlusMiusLEDs + LEDs);
        LEDminPin %= LEDs;
        int LEDmaxPin = round(LEDmiddlePin + PlusMiusLEDs + LEDs);
        LEDmaxPin %= LEDs;
        Mapping_NeoLight(LEDminPin, LEDmaxPin, 2 * PlusMiusLEDs + 1, minRGBedge, maxRGBedge, PerBright);
    }
}
