#include <Arduino.h>
#include "USE_LCD.h"
#include "USE_m5pico.h"
#include "USE_DSR1202.h"
#include "USE_Kicker.h"
#include "USE_BNO.h"
#include "USE_L3GD20H.h"
#include "USE_PIDv2.h"
#include "USE_ControlSW.h"
#include "USE_NeoPixel.h"

// メニューからどのアプリを使うか選択する関数
int Receive_Select = 0; // 返す値を格納する変数
int App_Select = 0;     // どのAppを選択したかどうか格納する変数
int SelectMenu()
{
    while (!ControlSW_right)
    {
        M5PICOget();    // M5-picoからの値を更新
        ControlSWget(); // コントロール基板のSWを更新

        display.drawBitmap(0, 0, Menu_bitmap, 128, 64, SSD1306_WHITE);
        if (Receive_Select == 0)
            display.drawBitmap(24, 4, Icon_MainProgram_bitmap, 56, 56, SSD1306_WHITE);
        if (Receive_Select == 1)
            display.drawBitmap(24, 4, Icon_PIDChecker_bitmap, 56, 56, SSD1306_WHITE);
        if (Receive_Select == 2)
            display.drawBitmap(24, 4, Icon_PS3Checker_bitmap, 56, 56, SSD1306_WHITE);

        if (App_Select == 0)
        {
            display.drawBitmap(105, 3, Press_app1_bitmap, 20, 27, SSD1306_WHITE);
            display.drawBitmap(105, 34, NoPress_app2_bitmap, 20, 27, SSD1306_WHITE);
            display.drawBitmap(100, 8, App_SelectStick_bitmap, 1, 17, SSD1306_WHITE);
        }
        else
        {
            display.drawBitmap(105, 3, NoPress_app1_bitmap, 20, 27, SSD1306_WHITE);
            display.drawBitmap(105, 34, Press_app2_bitmap, 20, 27, SSD1306_WHITE);
            display.drawBitmap(100, 39, App_SelectStick_bitmap, 1, 17, SSD1306_WHITE);
        }

        // SWのデータを管理
        if (ControlSW_left == 0)
        {
            display.drawBitmap(10, 28, Normal_LeftArrow_bitmap, 6, 9, SSD1306_WHITE);
        }
        else
        {
            display.drawBitmap(10, 28, Press_LeftArrow_bitmap, 6, 9, SSD1306_WHITE);
            App_Select = 0;                            // 別のアプリなので初期化
            Receive_Select = (Receive_Select + 1) % 3; // 足される
        }

        if (ControlSW_front)
        {
            // もともと0なら1を1なら0を代入
            if (App_Select == 0)
            {
                App_Select = 1;
            }
            else
            {
                App_Select = 0;
            }
        }

        // NeoPixel・LCDを表示
        LCD_Show(); // LCDを表示
        for (int i = 0; i < 40; i++)
        {
            SpeedTurn_NeoLight(10, 100);

            NeoPixel_Show();  // NeoPixelを表示
            delay(1);         // 1ms待つ
            NeoPixel_Clear(); // NeoPixelをクリア
        }
        LCD_Clear(); // LCDをクリア

    } // Startスイッチが押されるまで繰り返す

    return (2 * Receive_Select + App_Select); // 選択された数字を返す
}