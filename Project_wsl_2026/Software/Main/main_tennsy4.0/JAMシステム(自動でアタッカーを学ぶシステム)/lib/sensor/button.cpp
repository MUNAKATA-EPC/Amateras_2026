#include "button.hpp"

bool Button::init(uint8_t pin, uint8_t pinmode)
{
    _pin = pin;
    _pinmode = pinmode;

    pinMode(_pin, _pinmode);

    return true;
}

// チャタリング防止のための閾値（ミリ秒）
const unsigned long DEBOUNCE_DELAY = 20;

void Button::update()
{
    // 現在の生のピン状態を取得
    bool current_state = false;
    if (_pinmode == INPUT_PULLDOWN)
    {
        current_state = (digitalRead(_pin) == HIGH);
    }
    else if (_pinmode == INPUT_PULLUP)
    {
        current_state = (digitalRead(_pin) == LOW);
    }

    // チャタリング防止処理
    // 前回の生の状態と異なる場合、タイマーをリセット
    if (current_state != _last_raw_state)
    {
        _debounceTimer.reset();
    }

    // タイマーが閾値を超えたら、安定したとみなして値を更新
    if (_debounceTimer.msTime() > DEBOUNCE_DELAY)
    {
        if (current_state != _pressed)
        {
            _pressed = current_state;
        }
    }
    _last_raw_state = current_state;

    // 押し続け時間の計測
    if (_pressed)
    {
        if (!_oldpressed)
            _pushingTimer.reset();

        _pushing_time = _pushingTimer.msTime();
    }
    else
    {
        _pushing_time = 0;
    }

    // 離された瞬間の検出
    _released = (!_pressed && _oldpressed);

    // 過去の確定状態を保存
    _oldpressed = _pressed;
}

bool Button::isPushing() { return _pressed; }

bool Button::isReleased() { return _released; }

unsigned long Button::pushingTime() { return _pushing_time; } // 押されている時間を貸す

void Button::pushingTimeReset()
{
    _pushing_time = 0;     // 押されている時間をリセット
    _pushingTimer.reset(); // タイマーもリセット
}

Button enterButton; // 共通で使えるようにしておく
Button backButton;  // 共通で使えるようにしておく
Button rightButton; // 共通で使えるようにしておく
Button leftButton;  // 共通で使えるようにしておく
Button resetButton; // 共通で使えるようにしておく
