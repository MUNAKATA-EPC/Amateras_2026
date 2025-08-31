#include "multiplexer.hpp"

/*マルチプレクサからの情報を読み取る*/

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

    if (_enable_pin != -1) // もし-1ではないなら
        pinMode(_enable_pin, OUTPUT);
}

int Multiplexer::read(int index)
{
    index = constrain(index, 0, 15); // 一応丸める

    if (_enable_pin != -1)               // もし-1ではないなら
        digitalWrite(_enable_pin, LOW); // 読み込むときはLOWにする必要がある

    uint8_t shift_num;   // 分解に使う変数
    bool output_data[4]; // 分解して得たデータ格納用
    for (int i = 0; i < 4; i++)
    {
        shift_num = 1 << i; // 0001を左にiだけ移動させる

        if ((shift_num & index) > 0) // shift_numとindexの論理積が0以上なら
        {
            output_data[i] = true; // 1を代入
        }
        else
        {
            output_data[i] = false; // 0を代入
        }
    }
    // indexを2進数に分解、それぞれ出力する
    digitalWrite(_s0_pin, output_data[0]);
    digitalWrite(_s1_pin, output_data[1]);
    digitalWrite(_s2_pin, output_data[2]);
    digitalWrite(_s3_pin, output_data[3]);

    delayMicroseconds(_multiplexer_settle_microseconds); // ちょっと待つ

    int read_value = analogRead(_read_pin); // 読み取る

    if (_enable_pin != -1)              // もし-1ではないなら
        digitalWrite(_enable_pin, HIGH); // 読み込み終了なのでHIGHにする

    return read_value; // 読み取った値を返す
}