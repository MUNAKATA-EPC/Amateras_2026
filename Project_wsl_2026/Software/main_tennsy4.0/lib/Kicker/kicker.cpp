#include "kicker.hpp"

/* キッカーで蹴ったり貯めたりする */

int16_t fet1_kick_pin = 0;   // fet1につないだピン
int16_t fet2_charge_pin = 0; // fet2につないだピン

int16_t kicker_cooldown_time = 1000; // クールダウンタイム

Timer my_cooldown_timer;       // クールダウン用タイマー
bool kicker_first_call = true; // 最初の呼び出しかどうか

void kicker_set_fetpin(int16_t kick_pin, int16_t charge_pin)
{
    fet1_kick_pin = kick_pin;
    fet2_charge_pin = charge_pin;
}

void kicker_init(int16_t cooldown_time)
{
    pinMode(fet1_kick_pin, OUTPUT);
    pinMode(fet2_charge_pin, OUTPUT);

    kicker_cooldown_time = cooldown_time;

    my_cooldown_timer.reset();
}

void kicker_kick(bool kick_signal)
{
    if (kicker_first_call)
    {
        kicker_first_call = false;
        my_cooldown_timer.reset();
    }

    if (kick_signal && my_cooldown_timer.get_time() > kicker_cooldown_time)
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

static int16_t catchsensor_pin = 0;

void catchsensor_init(int16_t pin)
{
    catchsensor_pin = pin;
    pinMode(catchsensor_pin, INPUT);
}

unsigned int get_catchsensor()
{
    return analogRead(catchsensor_pin);
}
