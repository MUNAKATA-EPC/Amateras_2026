#include "test.hpp"

/*動作確認用のプログラムを実行する*/

void play_test(uint8_t test_mode)
{
    switch (test_mode)
    {
    case TEST_KICKER_MODE:
        // キッカーについて
        if (bno_reset_button.is_released() || lcd_right_button.is_released() || lcd_left_button.is_released() /*|| get_catchsensor()*/ ) // ボタンもしくはキャッチセンサーに反応があったら
        {
            kicker_kick(true);
        }
        else
        {
            kicker_kick(false);
        }

        break;

    case TEST_MONITOR_MODE:
        break;

    case TEST_PD_MODE:
        break;
    }
}