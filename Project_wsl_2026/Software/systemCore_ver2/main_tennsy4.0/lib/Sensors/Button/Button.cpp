#include "button.hpp"

/*ボタンを定義*/

void Button::init(uint8_t pin, uint8_t pinmode)
{
    _pin = pin;
    _pinmode = pinmode;

    pinMode(_pin, _pinmode);
    _count = 0;
}

void Button::update()
{
    bool pressed = false;

    if (_pinmode == INPUT_PULLDOWN)
    {
        pressed = (digitalRead(_pin) == HIGH); // ボタンが押されたか
    }
    else if (_pinmode == INPUT_PULLUP)
    {
        pressed = (digitalRead(_pin) == LOW); // ボタンが押されたか
    }

    if (pressed) // ボタンが押されている場合
    {
        if (_count <= 0)
            _pushingTimer.reset(); // 最初に押されたときにタイマーをリセット

        _count++;                              // カウントアップ
        _pushingTime = _pushingTimer.millis(); // タイマーからの時間を格納
    }
    else // ボタンが押されていない場合
    {
        if (_count <= 0) // 0もしくは負（-1）ならば
        {
            _count = 0; // 0にする
        }
        else //_button_countが正なら
        {
            _count = -1;                           // カウントアップされているので-1にする
            _pushingTime = _pushingTimer.millis(); // 最後の時間を記録
            _pushingTimer.reset();                 // 押されていないのでリセット
        }
    }
}

bool Button::isPushing()
{
    return _count > 0; // _button_countが正なら押されている
}

bool Button::isReleased()
{
    return _count == -1; // _button_countが-1なら離された直後
}

unsigned long Button::pushingTime()
{
    return _pushingTime; // 押されている時間を返す
}

void Button::pushingTimeReset()
{
    _pushingTime = 0;      // 押されている時間をリセット
    _pushingTimer.reset(); // タイマーもリセット
}
