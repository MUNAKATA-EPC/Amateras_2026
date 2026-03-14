#include "multiplexer.hpp"

void Multiplexer::set_pin(int s0_pin, int s1_pin, int s2_pin, int s3_pin, int read_pin, int enable_pin)
{
    _s0_pin = s0_pin;
    _s1_pin = s1_pin;
    _s2_pin = s2_pin;
    _s3_pin = s3_pin;
    _read_pin = read_pin;
    _enable_pin = enable_pin;
}

void Multiplexer::init(int multiplexer_settle_microseconds)
{
    _multiplexer_settle_microseconds = multiplexer_settle_microseconds;

    pinMode(_s0_pin, OUTPUT);
    pinMode(_s1_pin, OUTPUT);
    pinMode(_s2_pin, OUTPUT);
    pinMode(_s3_pin, OUTPUT);
    pinMode(_read_pin, INPUT);

    if (_enable_pin != -1)
    {
        pinMode(_enable_pin, OUTPUT);
        digitalWrite(_enable_pin, HIGH); // 初期状態は無効化(HIGH)
    }
}

int Multiplexer::read(int index)
{
    index = constrain(index, 0, 15); // 一応丸める

    if (_enable_pin != -1)
    {
        digitalWrite(_enable_pin, LOW); // 読み込むときはLOW
    }

    // indexを2進数に分解し、直接出力する（配列を省いて高速化）
    digitalWrite(_s0_pin, (index >> 0) & 1);
    digitalWrite(_s1_pin, (index >> 1) & 1);
    digitalWrite(_s2_pin, (index >> 2) & 1);
    digitalWrite(_s3_pin, (index >> 3) & 1);

    delayMicroseconds(_multiplexer_settle_microseconds); // ちょっと待つ

    int read_value = analogRead(_read_pin); // 読み取る

    if (_enable_pin != -1)
    {
        digitalWrite(_enable_pin, HIGH); // 読み込み終了でHIGH
    }

    return read_value; // 読み取った値を返す
}