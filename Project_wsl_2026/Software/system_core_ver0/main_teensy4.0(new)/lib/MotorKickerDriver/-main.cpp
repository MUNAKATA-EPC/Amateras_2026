#include "Arduino.h"
#include "MySettings.h"
#include "DSR1202.h"
#include "Kicker.h"

/*DSR1202*/

void DSR1202begin()
{
    DSR1202_SERIAL.begin(DSR1202_SERIALSPEED);

    DSR1202move(0, 0, 0, 0); // モータを停止する
}

void DSR1202move(int a, int b, int c, int d)
{
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

    DSR1202_SERIAL.println(StringA + StringB + StringC + StringD);
}

/*Kicker*/

long now_KickerTime = 0, old_KickerTime = 0; // キッカーのクールダウンの用のタイマー

void Kickerbegin()
{
    pinMode(FET1_KICKPIN, OUTPUT);   // OUTPUTに設定
    pinMode(FET2_CHARGEPIN, OUTPUT); // OUTPUTに設定
}

void KickerPlay(bool kick)
{
    now_KickerTime = millis() - old_KickerTime; // キックした最終時刻からどのくらい経ったか

    if (kick && now_KickerTime >= KICKER_COOLDOWNTIME)
    {
        digitalWrite(FET1_KICKPIN, HIGH);  // Kick信号を流す
        digitalWrite(FET2_CHARGEPIN, LOW); // Charge信号を切る

        old_KickerTime = millis(); // キックした最終時刻を記録
    }
    else
    {
        digitalWrite(FET1_KICKPIN, LOW);    // Kick信号を切る
        digitalWrite(FET2_CHARGEPIN, HIGH); // Charge信号を流す
    }
}

void KickerCharge(bool Charge)
{
    if (Charge)
    {
        digitalWrite(FET2_CHARGEPIN, HIGH); // Charge信号を流す
    }
    else
    {
        digitalWrite(FET2_CHARGEPIN, LOW); // Charge信号を切る
    }
}