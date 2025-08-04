#include "kicker.hpp"

/*キッカーで蹴ったり貯めたりする*/

int fet1_kick_pin = 0;   // fet1につないだピン格納用
int fet2_charge_pin = 0; // fet2につないだピン格納用

int kicker_cooldown_time = 1000; // クールダウンタイム格納用

Timer my_kicker_timer;         // クールダウン用のタイマー
bool kicker_first_call = true; // 最初の呼び出しかどうか

void kicker_set_fetpin(int kick_pin, int charge_pin)
{
    fet1_kick_pin = kick_pin;
    fet2_charge_pin = charge_pin;
}

void kicker_init(int cooldown_time)
{
    pinMode(fet1_kick_pin, OUTPUT);   // ピンを設定
    pinMode(fet2_charge_pin, OUTPUT); // ピンを設定

    kicker_cooldown_time = cooldown_time;

    my_kicker_timer.reset(); // 一応リセットする
}

void kicker_kick(bool kick_signal)
{
    if (kicker_first_call) // もし最初の呼び出しなら
    {
        kicker_first_call = false;
        my_kicker_timer.reset(); // リセットする
    }

    if (kick_signal && my_kicker_timer.get_time() > kicker_cooldown_time)
    {
        digitalWrite(fet1_kick_pin, HIGH); // キックする
        my_kicker_timer.reset();           // クールダウン開始
    }
    else
    {
        digitalWrite(fet1_kick_pin, LOW); // キックしない
    }
}

void kicker_charge(bool charge_signal)
{
    if (charge_signal)
    {
        digitalWrite(fet2_charge_pin, HIGH); // チャージする
    }
    else
    {
        digitalWrite(fet2_charge_pin, LOW); // チャージしない
    }
}

/*キャッチセンサー*/

int catchsensor_pin = 0; // キャッチセンサーのピン格納用

void catchsensor_init(int pin)
{
    catchsensor_pin = pin;
    pinMode(catchsensor_pin, INPUT); // ピン設定
}

bool get_catchsensor()
{
    return digitalRead(catchsensor_pin); // キャッチセンサーの状態を返す
}