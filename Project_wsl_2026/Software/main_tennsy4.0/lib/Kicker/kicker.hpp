#ifndef KICKER_HPP
#define KICKER_HPP

#include <Arduino.h>
#include "timer.hpp"

/* キッカー */

// キッカーのピン番号を設定
void kicker_set_fetpin(int16_t kick_pin, int16_t charge_pin);

// キッカーのクールダウンの時間を設定・開始
void kicker_init(int16_t cooldown_time);

// キックする（クールダウンタイム中は蹴らない）
void kicker_kick(bool kick_signal);

/* キャッチセンサー */

// キャッチセンサーのピン設定・開始
void catchsensor_init(int16_t pin);

// キャッチセンサーの値を取得
unsigned int get_catchsensor();

#endif