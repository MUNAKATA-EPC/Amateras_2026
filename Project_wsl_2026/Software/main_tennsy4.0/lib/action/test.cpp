#include "test.hpp"

void play_test(int test_mode)
{
    switch (test_mode)
    {
    case TEST_KICKER_MODE:
        // キッカーについて
        kicker_charge(true);                                     // 常にチャージはする
        if (lcd_enter_button.is_released() || get_catchsensor()) // 決定ボタンもしくはキャッチセンサーに反応があったら
        {
            kicker_kick(true);
        }
        else
        {
            kicker_kick(false);
        }

        break;

    case TEST_PD_GYRO_MODE:
        break;

    case TEST_PD_CAM_MODE:
        break;

    default:
        Serial.println("error");
        break;
    }
}