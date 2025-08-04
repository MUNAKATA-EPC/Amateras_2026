#include "button.hpp"

/*ボタンを定義*/
Button bno_reset_button; // BNO055のリセットボタン用

Button lcd_enter_button; // 決定ボタン
Button lcd_right_button; // 右ボタン
Button lcd_left_button;  // 左ボタン

void Button::set_pin(int pin, int pinmode)
{
    _button_pin = pin;         // ピン番号を格納
    _button_pinmode = pinmode; // ピンモードを格納
}

void Button::init()
{
    pinMode(_button_pin, _button_pinmode); // ピンモードを設定
    _button_count = 0;                     // ボタンのカウントをリセット
}

void Button::update()
{
    if (_button_pinmode == INPUT_PULLDOWN)
    {
        if (digitalRead(_button_pin) == HIGH) // ボタンが押されたなら
        {
            _button_count++; // カウントアップ
        }
        else // ボタンが押されていないなら
        {
            if (_button_count <= 0) // 0もしくは負（-1）ならば
            {
                _button_count = 0; // 0にする
            }
            else //_button_countが正なら
            {
                _button_count = -1; // カウントアップされているので-1にする
            }
        }
    }
    else if (_button_pinmode == INPUT_PULLUP)
    {
        if (digitalRead(_button_pin) == LOW) // ボタンが押されたなら
        {
            _button_count++; // カウントアップ
        }
        else // ボタンが押されていないなら
        {
            if (_button_count <= 0) // 0もしくは負（-1）ならば
            {
                _button_count = 0; // 0にする
            }
            else //_button_countが正なら
            {
                _button_count = -1; // カウントアップされているので-1にする
            }
        }
    }
}

bool Button::is_pushing()
{
    return _button_count > 0; // _button_countが正の状況はボタンが押されていると判断
}

bool Button::is_released()
{
    return _button_count == -1; // _button_countが-1の状況はボタンが離されて直後の状況と判断
}