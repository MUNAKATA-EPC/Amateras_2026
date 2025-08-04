#ifndef TOGGLE_HPP
#define TOGGLE_HPP

#include <Arduino.h>

class Toggle
{
private:
    int _toggle_pin = 0; // ボタンのピン番号格納用

public:
    // 使うボタンのピン番号を設定する
    void set_pin(int pin);

    // ボタンを初期化する（ピンモードを設定する）
    void init();

    // ボタンがトグルスイッチがONになっているかどうか
    bool is_turn_on();
};

#endif