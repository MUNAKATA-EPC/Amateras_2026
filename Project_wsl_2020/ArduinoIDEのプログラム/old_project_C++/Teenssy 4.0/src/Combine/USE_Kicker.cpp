#include <Arduino.h>

// キックするピンとチャージするピンを格納
int kick_pin, charge_pin;

void Kickerbegin(int fet1, int fet2)
{
    charge_pin = fet1;
    kick_pin = fet2;
}

void CheckKick(int Kickdata)
{
    if (Kickdata > 0)
    {
        digitalWrite(kick_pin, HIGH);
        digitalWrite(charge_pin, LOW);
    }
    else
    {
        digitalWrite(kick_pin, LOW);
        digitalWrite(charge_pin, HIGH);
    }
}