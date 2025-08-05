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

Timer my_ui_timer;         // UI用のタイマー
bool ui_first_call = true; // 関数が最初に呼び出されたかを読む

/*Actionについてのメモ*/
// 攻撃する
// #define ACTION_ATTACKER 0
// 守備する
// #define ACTION_DEFENDER 1
// テスト(確認)する
// #define ACTION_TEST 2
int action_number = ACTION_ATTACKER; // とりあえず攻撃するを格納
bool action_decided = false;         // Actionをユーザーが決定されたかどうか

/*modeについてのメモ*/
// 攻撃or守備をするとき
// PD制御でジャイロだけ使うモード
// #define PD_USE_ONLY_GYRO_MODE 0
// PD制御でカメラも使うモード
// #define PD_USE_CAM_MODE 1

// テストをするとき
// キッカーの動作を確認するモード
// #define TEST_KICKER_MODE 0
// ジャイロでのPD制御のテストを行うモード
// #define TEST_PD_GYRO_MODE 1
// カメラでのPD制御のテストを行うモード
// #define TEST_PD_CAM_MODE 2
int mode_number = 0;
bool mode_decided = false; // modeをユーザーが決定されたかどうか

void ui_process()
{
    lcd_enter_button.update(); // ボタンの状況を更新
    lcd_left_button.update();  // ボタンの状況を更新
    lcd_right_button.update(); // ボタンの状況を更新

    // 右ボタンが押された場合
    if (lcd_right_button.is_released())
    {
        if (!action_decided) // まだactionが決められていない
            action_number = (action_number + 1 + 3) % 3;
        else if (!mode_decided) // まだmodeが決められていない
        {
            if (action_number == ACTION_ATTACKER || action_number == ACTION_DEFENDER) // 攻撃か守備なら
                mode_number = (mode_number + 1 + 2) % 2;
            else // 確認なら
                mode_number = (mode_number + 1 + 3) % 3;
        }
    }
    // 左ボタンが押された場合
    if (lcd_left_button.is_released())
    {
        if (!action_decided) // まだactionが決められていない
            action_number = (action_number - 1 + 3) % 3;
        else if (!mode_decided) // まだmodeが決められていない
        {
            if (action_number == ACTION_ATTACKER || action_number == ACTION_DEFENDER) // 攻撃か守備なら
                mode_number = (mode_number - 1 + 2) % 2;
            else // 確認なら
                mode_number = (mode_number - 1 + 3) % 3;
        }
    }

    // 決定ボタンが押された場合
    if (lcd_enter_button.is_released())
    {
        if (lcd_enter_button.get_pushing_time() >= 800) // 800ms以上押されていたら
        {
            if (action_decided && mode_decided) // actionもmodeも決定されたなら
            {
                mode_decided = false; // modeをもう一度決めれるようにする
            }
            else if (action_decided) // actionが決められていたら
            {
                action_decided = false; // actinoをもう一度決めれるようにする
            }
        }
        else // それ未満なら
        {
            if (!action_decided) // まだactionが決められていない
                action_decided = true;
            else if (!mode_decided) // まだmodeが決められていない
                mode_decided = true;
        }
    }

    /*LCDに表示*/
    if (ui_first_call)
    {
        ui_first_call = false; // 次回からは初めてではない
        my_ui_timer.reset();   // リセットする
    }
    if (my_ui_timer.get_time() >= 50) // 50ms経ってactionまたはmodeが決められていないなら
    {
        my_ui_timer.reset(); // リセット

        if (!action_decided || !mode_decided) // まだ選ばれていないなら
        {
            SSD1306_clear(); // LCDを初期化する

            // actionについて
            switch (action_number)
            {
            case ACTION_ATTACKER:
                SSD1306_write(1, 0, 0, "Action-Attacker", false);

                if (action_decided) // actionが決められた
                {
                    // modeについて
                    switch (mode_number)
                    {
                    case PD_USE_ONLY_GYRO_MODE:
                        SSD1306_write(1, 6, 10, "PD : Use only gyro", false);

                        play_lcd_print(GYRO_CHECK_WITH_LCD, 1, 25);
                        break;

                    case PD_USE_CAM_MODE:
                        SSD1306_write(1, 6, 10, "PD : Use cam", false);

                        play_lcd_print(CAM_CHECK_WITH_LCD, 1, 25);
                        break;
                    }
                }
                break;

            case ACTION_DEFENDER:
                SSD1306_write(1, 0, 0, "Action-Defender", false);

                if (action_decided) // actionが決められた
                {
                    // modeについて
                    switch (mode_number)
                    {
                    case PD_USE_ONLY_GYRO_MODE:
                        SSD1306_write(1, 6, 10, "PD : Use only gyro", false);

                        play_lcd_print(GYRO_CHECK_WITH_LCD, 1, 25);
                        break;

                    case PD_USE_CAM_MODE:
                        SSD1306_write(1, 6, 10, "PD : Use cam", false);

                        play_lcd_print(CAM_CHECK_WITH_LCD, 1, 25);
                        break;
                    }
                }
                break;

            case ACTION_TEST:
                SSD1306_write(1, 0, 0, "Action-Test", false);

                if (action_decided) // actionが決められた
                {
                    // modeについて
                    switch (mode_number)
                    {
                    case TEST_KICKER_MODE:
                        SSD1306_write(1, 6, 10, "check : kicker", false);
                        break;

                    case TEST_PD_GYRO_MODE:
                        SSD1306_write(1, 6, 10, "check : PD only gyro", false);

                        play_lcd_print(GYRO_CHECK_WITH_LCD, 1, 25);
                        break;

                    case TEST_PD_CAM_MODE:
                        SSD1306_write(1, 6, 10, "check : PD cam", false);

                        play_lcd_print(CAM_CHECK_WITH_LCD, 1, 25);
                        break;
                    }
                }
                break;
            }

            SSD1306_show(); // LCDに表示する
        }
        else // 決定された後
        {
            SSD1306_clear(); // LCDを初期化する

            SSD1306_write(2, 0, 0, "Running", false);
            play_lcd_print(IR_LINE_GYRO_CHECK_WITH_LCD, 0, 16);

            SSD1306_show(); // LCDに表示する
        }
    }
}

bool is_now_selecting_ui()
{
    return (!action_decided || !mode_decided);
}

int get_selected_ui_action()
{
    return action_number;
}

int get_selected_ui_mode()
{
    return mode_number;
}