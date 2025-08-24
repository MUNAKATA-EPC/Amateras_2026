#include "kicker.hpp"

/* キッカーで蹴ったり貯めたりする */

int fet1_kick_pin = 0;   // fet1につないだピン
int fet2_charge_pin = 0; // fet2につないだピン

int kicker_cooldown_time = 1000; // クールダウンタイム

Timer my_cooldown_timer; // クールダウン用タイマー

void kicker_set_fetpin(int kick_pin, int charge_pin)
{
    fet1_kick_pin = kick_pin;
    fet2_charge_pin = charge_pin;
}

void kicker_init(int cooldown_time)
{
    pinMode(fet1_kick_pin, OUTPUT);
    pinMode(fet2_charge_pin, OUTPUT);

    kicker_cooldown_time = cooldown_time;

    my_cooldown_timer.reset();
}

void kicker_kick(bool kick_signal)
{
    if (kick_signal && (my_cooldown_timer.get_time() > kicker_cooldown_time || !my_cooldown_timer.is_called_this_timer()))
    {
        digitalWrite(fet1_kick_pin, HIGH);
        digitalWrite(fet2_charge_pin, LOW);
        my_cooldown_timer.reset();
    }
    else if (my_cooldown_timer.get_time() <= 50)
    {
        digitalWrite(fet1_kick_pin, HIGH);
        digitalWrite(fet2_charge_pin, LOW);
    }
    else
    {
        digitalWrite(fet1_kick_pin, LOW);
        digitalWrite(fet2_charge_pin, HIGH);
    }
}

/* キャッチセンサー */

static int catchsensor_pin = 0;

void catchsensor_init(int pin)
{
    catchsensor_pin = pin;
    pinMode(catchsensor_pin, INPUT);
}

unsigned int get_catchsensor()
{
    return analogRead(catchsensor_pin);
}
