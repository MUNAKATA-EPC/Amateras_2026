#ifndef BUTTON_HPP
#define BUTTON_HPP

#include <Arduino.h>

class Button
{
private:
    int _button_pin = 0;                // ボタンのピン番号格納用
    int _button_pinmode = INPUT_PULLUP; // ボタンのピンモード格納用

    int _button_count = 0; // ボタンが押されている回数格納用

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
};

/*ボタンの実態を定義*/
extern Button bno_reset_button; // BNO055のリセットボタン用

extern Button lcd_enter_button; // 決定ボタン
extern Button lcd_right_button; // 右ボタン
extern Button lcd_left_button;  // 左ボタン

#endif
