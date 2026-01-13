#pragma once
#include <Arduino.h>

// PWMの関数
#include "hardware/pwm.h" // Pico SDKのPWMヘッダをインクルード

void pwmSet(uint8_t pin, int hz, int resolution)
{
    gpio_set_function(pin, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(pin);

    // 周波数の計算と設定
    // システムクロック(125MHz) / (周波数 * 段階数)　で計算
    float clkdiv = 125000000.0f / (hz * (1 << resolution));
    pwm_set_clkdiv(slice_num, clkdiv);
    pwm_set_wrap(slice_num, (1 << resolution) - 1);
    pwm_set_enabled(slice_num, true);
}

void pwmWrite(uint pin, uint16_t value)
{
    pwm_set_gpio_level(pin, value);
}

// モーター制御のクラス
class Motor
{
private:
    uint8_t _inA_pin, _inB_pin, _pwm_pin;

public:
    void setPin(uint8_t inA_pin, uint8_t inB_pin, uint8_t pwm_pin)
    {
        _inA_pin = inA_pin;
        _inB_pin = inB_pin;
        _pwm_pin = pwm_pin;
        pinMode(_inA_pin, OUTPUT);
        pinMode(_inB_pin, OUTPUT);

        // PWMピンの設定
        pwmSet(_pwm_pin, 20000, 10); // 20kHz、10ビット解像度
    }

    void move(int power)
    {
        power = constrain(power, -100, 100);
        // 0-100の入力を0-1023に変換
        uint16_t output = abs(power) * 1023 / 100;

        if (power >= 0)
        {
            digitalWrite(_inA_pin, HIGH);
            digitalWrite(_inB_pin, LOW);
        }
        else
        {
            digitalWrite(_inA_pin, LOW);
            digitalWrite(_inB_pin, HIGH);
        }

        pwmWrite(_pwm_pin, output);
    }

    void stop()
    {
        digitalWrite(_inA_pin, LOW);
        digitalWrite(_inB_pin, LOW);
        pwmWrite(_pwm_pin, 0);
    }
};