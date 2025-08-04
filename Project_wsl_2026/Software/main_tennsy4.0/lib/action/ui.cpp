#include "ui.hpp"

/*uiを実行する*/

int lcd_enter_pin = 0, lcd_left_pin = 0, lcd_right_pin = 0;             // LCDを動かすためのButtonのピン番号格納用
int lcd_enter_pinmode = 0, lcd_left_pinmode = 0, lcd_right_pinmode = 0; // LCDを動かすためのButtonのピンモード格納用

void ui_set_lcdpin(int enter_pin, int enter_pinmode, int left_pin, int left_pinmode, int right_pin, int right_pinmode)
{
    lcd_enter_pin = enter_pin;
    lcd_left_pin = left_pin;
    lcd_right_pin = right_pin;

    lcd_enter_pinmode = enter_pinmode;
    lcd_left_pinmode = left_pinmode;
    lcd_right_pinmode = right_pinmode;
}

void ui_init()
{
    // ピンを設定
    lcd_enter_button.set_pin(lcd_enter_pin, lcd_enter_pinmode);
    lcd_enter_button.init();
    lcd_left_button.set_pin(lcd_left_pin, lcd_left_pinmode);
    lcd_left_button.init();
    lcd_right_button.set_pin(lcd_right_pin, lcd_right_pinmode);
    lcd_right_button.init();
}

void ui_select()
{
    lcd_enter_button.update(); // 更新する
    lcd_left_button.update();  // 更新する
    lcd_right_button.update(); // 更新する
}

int get_ui_action() {}

int get_ui_pd_mode() {}