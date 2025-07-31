#include "Arduino.h"
#include "USE_DSR1202.h"

// どのシリアルを使うか
int PinNo = 0;

// モータ調整用のゲイン値
float Gain_a = 1.0, Gain_b = 1.0, Gain_c = 1.0, Gain_d = 1.0;

void DSR1202begin(int pin)
{
    PinNo = pin;

    if (PinNo == 1)
    {
        Serial1.begin(115200);
    }
    else if (PinNo == 2)
    {
        Serial2.begin(115200);
    }
    else if (PinNo == 3)
    {
        Serial3.begin(115200);
    }
    else
    {
        Serial.begin(115200);
    }
}

void DSR1202motorGain(float a, float b, float c, float d)
{
    Gain_a = a;
    Gain_b = b;
    Gain_c = c;
    Gain_d = d;
}

void DSR1202move(int a, int b, int c, int d)
{
    a = int(a * Gain_a);
    b = int(b * Gain_b);
    c = int(c * Gain_c);
    d = int(d * Gain_d);
    
    a = constrain(a, -100, 100);
    b = constrain(b, -100, 100);
    c = constrain(c, -100, 100);
    d = constrain(d, -100, 100);

    int val = 0;
    int input[4] = {a, b, c, d};
    String StringA = "0";
    String StringB = "0";
    String StringC = "0";
    String StringD = "0";
    String StringX = "0";
    String StringY = "0";
    String StringZ = "0";
    String String0 = "0";

    StringX = String(1);
    val = input[0];
    if (val < 0 && val >= -100)
    {
        StringY = String('R');
        val = abs(val);
        StringZ = String(val);
    }
    else if (val >= 0 && val <= 100)
    {
        StringY = String('F');
        StringZ = String(val);
    }
    else
    {
        StringY = String('F');
        StringZ = String(0);
    }
    if (val < 10)
    {
        String0 = String("00");
        StringZ = String0 + StringZ;
    }
    else if (val < 100)
    {
        String0 = String('0');
        StringZ = String0 + StringZ;
    }
    else
    {
        String0 = String("");
    }
    StringA = StringX + StringY + StringZ;

    StringX = String(2);
    val = input[1];
    if (val < 0 && val >= -100)
    {
        StringY = String('R');
        val = abs(val);
        StringZ = String(val);
    }
    else if (val >= 0 && val <= 100)
    {
        StringY = String('F');
        StringZ = String(val);
    }
    else
    {
        StringY = String('F');
        StringZ = String(0);
    }
    if (val < 10)
    {
        String0 = String("00");
        StringZ = String0 + StringZ;
    }
    else if (val < 100)
    {
        String0 = String('0');
        StringZ = String0 + StringZ;
    }
    else
    {
    }
    StringB = StringX + StringY + StringZ;

    StringX = String(3);
    val = input[2];
    if (val < 0 && val >= -100)
    {
        StringY = String('R');
        val = abs(val);
        StringZ = String(val);
    }
    else if (val >= 0 && val <= 100)
    {
        StringY = String('F');
        StringZ = String(val);
    }
    else
    {
        StringY = String('F');
        StringZ = String(0);
    }
    if (val < 10)
    {
        String0 = String("00");
        StringZ = String0 + StringZ;
    }
    else if (val < 100)
    {
        String0 = String('0');
        StringZ = String0 + StringZ;
    }
    else
    {
    }
    StringC = StringX + StringY + StringZ;

    StringX = String(4);
    val = input[3];
    if (val < 0 && val >= -100)
    {
        StringY = String('R');
        val = abs(val);
        StringZ = String(val);
    }
    else if (val >= 0 && val <= 100)
    {
        StringY = String('F');
        StringZ = String(val);
    }
    else
    {
        StringY = String('F');
        StringZ = String(0);
    }
    if (val < 10)
    {
        String0 = String("00");
        StringZ = String0 + StringZ;
    }
    else if (val < 100)
    {
        String0 = String('0');
        StringZ = String0 + StringZ;
    }
    else
    {
    }
    StringD = StringX + StringY + StringZ;

    if (PinNo == 1)
    {
        Serial1.println(StringA + StringB + StringC + StringD);
    }
    else if (PinNo == 2)
    {
        Serial2.println(StringA + StringB + StringC + StringD);
    }
    else if (PinNo == 3)
    {
        Serial3.println(StringA + StringB + StringC + StringD);
    }
    else
    {
        Serial.println(StringA + StringB + StringC + StringD);
    }
}