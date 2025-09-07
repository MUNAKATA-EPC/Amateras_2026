#include "button.hpp"

Button::Button(uint8_t pin, uint8_t pinmode)
{
    _pin = pin;
    _pinmode = pinmode;
}

void Button::begin()
{
    pinMode(_pin, _pinmode);
}

void Button::update()
{
    if (_pinmode == INPUT_PULLDOWN)
    {
        _pressed = (digitalRead(_pin) == HIGH);
    }
    else if (_pinmode == INPUT_PULLUP)
    {
        _pressed = (digitalRead(_pin) == LOW);
    }

    if (_pressed)
    {
        if (!_oldpressed)
            _pushingTimer.reset();

        _pushingTime = _pushingTimer.msTime();
    }
    else
    {
        _pushingTime = 0;
    }

    // 変化を検出
    _released = (!_pressed && _oldpressed);

    // 過去の状態を保存
    _oldpressed = _pressed;
}

bool Button::isPushing()
{
    return _pressed;
}

bool Button::isReleased()
{
    return _released;
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
