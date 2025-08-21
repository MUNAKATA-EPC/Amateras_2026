#include "button.hpp"

/*ボタンを定義*/
Button bno_reset_button; // BNO055のリセットボタン用
Button lcd_enter_button; // 決定ボタン
Button lcd_right_button; // 右ボタン
Button lcd_left_button;  // 左ボタン

void Button::set_pin(uint8_t pin, uint8_t pinmode)
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
    bool pressed = false;

    if (_button_pinmode == INPUT_PULLDOWN)
    {
        pressed = (digitalRead(_button_pin) == HIGH); // ボタンが押されたか
    }
    else if (_button_pinmode == INPUT_PULLUP)
    {
        pressed = (digitalRead(_button_pin) == LOW); // ボタンが押されたか
    }

    if (pressed) // ボタンが押されている場合
    {
        if (_button_count <= 0)
            _pushing_timer.reset(); // 最初に押されたときにタイマーをリセット

        _button_count++; // カウントアップ
        _pushing_time = _pushing_timer.get_time(); // タイマーからの時間を格納
    }
    else // ボタンが押されていない場合
    {
        if (_button_count <= 0) // 0もしくは負（-1）ならば
        {
            _button_count = 0; // 0にする
        }
        else //_button_countが正なら
        {
            _button_count = -1; // カウントアップされているので-1にする
            _pushing_time = _pushing_timer.get_time(); // 最後の時間を記録
            _pushing_timer.reset();                    // 押されていないのでリセット
        }
    }
}

bool Button::is_pushing()
{
    return _button_count > 0; // _button_countが正なら押されている
}

bool Button::is_released()
{
    return _button_count == -1; // _button_countが-1なら離された直後
}

uint16_t Button::get_pushing_time()
{
    return _pushing_time; // 押されている時間を返す
}

void Button::reset_pushing_time()
{
    _pushing_time = 0;       // 押されている時間をリセット
    _pushing_timer.reset();  // タイマーもリセット
}
