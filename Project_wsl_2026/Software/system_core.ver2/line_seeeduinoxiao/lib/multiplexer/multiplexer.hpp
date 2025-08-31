#ifndef MULTIPLEXCER_HPP
#define MULTIPLEXCER_HPP

#include <Arduino.h>

class Multiplexer
{
private:
    int _s0_pin = 0;   // マルチプレクサのs0のピン番号格納用
    int _s1_pin = 0;   // マルチプレクサのs1のピン番号格納用
    int _s2_pin = 0;   // マルチプレクサのs2のピン番号格納用
    int _s3_pin = 0;   // マルチプレクサのs3のピン番号格納用
    int _read_pin = 0; // マルチプレクサのreadピン番号格納用

    int _enable_pin = -1; // マルチプレクサのeのピン番号格納用

    int _multiplexer_settle_microseconds; // マルチプレクサの出力を読むまでの待機時間格納用

public:
    // 使うピン番号を設定する(enable_pinについて使わない場合は-1を入力)
    void set_pin(int s0_pin, int s1_pin, int s2_pin, int s3_pin, int read_pin, int enable_pin);

    // マルチプレクサ初期化(ピンを初期化)
    void init(int multiplexer_settle_microseconds);

    // マルチプレクサから情報を読み取る
    unsigned int read(int index);
};

#endif
