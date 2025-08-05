#ifndef KICKER_HPP
#define KICKER_HPP

#include <Arduino.h>
#include "timer.hpp"

/*キッカー*/

// キッカーのピン番号を設定
void kicker_set_fetpin(int kick_pin, int charge_pin);
// キッカーのクールダウンの時間を設定・開始
void kicker_init(int cooldown_time);

// キックする（クールダウンタイム中は蹴らない）
void kicker_kick(bool kick_signal);

/*キャッチセンサー*/

// キャッチセンサーのピン設定・開始
void catchsensor_init(int pin);

// キャッチセンサーは反応しているかどうか
bool get_catchsensor();

#endif