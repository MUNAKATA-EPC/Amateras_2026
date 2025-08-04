#include "toggle.hpp"

void Toggle::set_pin(int pin)
{
    _toggle_pin = pin;
}

void Toggle::init()
{
    pinMode(_toggle_pin, INPUT); // ピンを設定する
}

bool Toggle::is_turn_on()
{
    return digitalRead(_toggle_pin); // トグルスイッチの状況を返す
}