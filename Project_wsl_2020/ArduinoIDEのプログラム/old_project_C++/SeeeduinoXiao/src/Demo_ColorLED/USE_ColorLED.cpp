#include <Arduino.h>
#include "USE_ColorLED.h"

int redPin, bluePin, greenPin; // それぞれのピンを指定

void ColorLEDbegin(int red, int blue, int green)
{
    // それぞれの色を担当するピンを変数に格納
    redPin = red;
    pinMode(redPin, OUTPUT); // OUTPUTに設定
    bluePin = blue;
    pinMode(bluePin, OUTPUT); // OUTPUTに設定
    greenPin = green;
    pinMode(greenPin, OUTPUT); // OUTPUTに設定
}

void LEDlightup(int red, int blue, int green)
{
    // それぞれ光らせる
    analogWrite(redPin, red);
    analogWrite(bluePin, blue);
    analogWrite(greenPin, green);
}

void LEDdeg(float h, float s, float v)
{
    if (h == 360)
        h = 0;

    float r = 0, g = 0, b = 0; // ←ここで初期化しておく

    int i = int(h / 60.0) % 6;
    float f = h / 60.0 - i;
    float p = v * (1.0 - s);
    float q = v * (1.0 - f * s);
    float t = v * (1.0 - (1.0 - f) * s);

    switch (i)
    {
    case 0:
        r = v;
        g = t;
        b = p;
        break;
    case 1:
        r = q;
        g = v;
        b = p;
        break;
    case 2:
        r = p;
        g = v;
        b = t;
        break;
    case 3:
        r = p;
        g = q;
        b = v;
        break;
    case 4:
        r = t;
        g = p;
        b = v;
        break;
    case 5:
        r = v;
        g = p;
        b = q;
        break;
    }

    // 0～1 の範囲を 0～255 に変換
    if (h == -1)
    {
        analogWrite(redPin, 0);
        analogWrite(greenPin, 0);
        analogWrite(bluePin, 0);
    }
    else
    {
        analogWrite(redPin, int(r * 255));
        analogWrite(greenPin, int(g * 255));
        analogWrite(bluePin, int(b * 255));
    }
}