#ifndef BUTTON_HPP
#define BUTTON_HPP

#include <Arduino.h>
#include "timer.hpp"

class Button
{
private:
    int _button_pin = 0;                // ボタンのピン番号格納用
    int _button_pinmode = INPUT_PULLUP; // ボタンのピンモード格納用

    int _button_count = 0; // ボタンが押されている回数格納用

    Timer _pushing_timer;  // 押されている時間計測用
    int _pushing_time = 0; // 押されている間の時間格納用

public:
    // 使うボタンのピン番号・ピンモードを設定する
    void set_pin(int pin, int pinmode);

    // ボタンを初期化する（ピンモードを設定する）
    void init();

    // ボタンの状況を確認し、計算する
    void update();

    // ボタンが押されているかどうか
    bool is_pushing();

    // ボタンが離されたかどうか
    bool is_released();

    // 前回ボタンがどのくらい押されていた、または今押されているボタンが今までどのくらい押され続けてきたか出力
    int get_pushing_time();

    // リセットする
    void reset_pushing_time();
};

/*ボタンの実態を定義*/
extern Button bno_reset_button; // BNO055のリセットボタン用

extern Button lcd_enter_button; // 決定ボタン
extern Button lcd_right_button; // 右ボタン
extern Button lcd_left_button;  // 左ボタン

#endif
