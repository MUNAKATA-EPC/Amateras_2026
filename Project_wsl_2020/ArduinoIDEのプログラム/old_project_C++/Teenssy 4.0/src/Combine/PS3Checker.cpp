#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include "USE_LCD.h"
#include "USE_m5pico.h"
#include "USE_Kicker.h"

// App1_PS3Checkerを起動させる関数
void App1_PS3Checker()
{
    while (!SW_cross)
    {
        M5PICOget(); // M5-picoからの値を更新

        // ホーム画面の配置
        display.drawBitmap(0, 0, HomeView_PS3CheckerApp1_bitmap, 128, 64, SSD1306_WHITE);
        // 十字キーの配置
        if (SW_up)
        {
            display.drawBitmap(14, 16, Normal_0cross_bitmap, 12, 16, SSD1306_WHITE);
        }
        else
        {
            display.drawBitmap(14, 16, Press_0cross_bitmap, 12, 16, SSD1306_WHITE);
        }
        if (SW_right)
        {
            display.drawBitmap(20, 26, Normal_90cross_bitmap, 16, 12, SSD1306_WHITE);
        }
        else
        {
            display.drawBitmap(20, 26, Press_90cross_bitmap, 16, 12, SSD1306_WHITE);
        }

        if (SW_down)
        {
            display.drawBitmap(14, 32, Normal_180cross_bitmap, 12, 16, SSD1306_WHITE);
        }
        else
        {
            display.drawBitmap(14, 32, Press_180cross_bitmap, 12, 16, SSD1306_WHITE);
        }

        if (SW_left)
        {
            display.drawBitmap(4, 26, Normal_270cross_bitmap, 16, 12, SSD1306_WHITE);
        }
        else
        {
            display.drawBitmap(4, 26, Press_270cross_bitmap, 16, 12, SSD1306_WHITE);
        }

        // 記号キーの配置
        if (SW_triangle)
        {
            display.drawBitmap(102, 16, Normal_triangle_bitmap, 12, 16, SSD1306_WHITE);
        }
        else
        {
            display.drawBitmap(102, 16, Press_triangle_bitmap, 12, 16, SSD1306_WHITE);
        }
        if (SW_circle)
        {
            display.drawBitmap(108, 26, Normal_circle_bitmap, 16, 12, SSD1306_WHITE);
        }
        else
        {
            display.drawBitmap(108, 26, Press_circle_bitmap, 16, 12, SSD1306_WHITE);
        }
        if (SW_cross)
        {
            display.drawBitmap(102, 32, Normal_cross_bitmap, 12, 16, SSD1306_WHITE);
        }
        else
        {
            display.drawBitmap(102, 32, Press_cross_bitmap, 12, 16, SSD1306_WHITE);
        }
        if (SW_square)
        {
            display.drawBitmap(92, 26, Normal_square_bitmap, 16, 12, SSD1306_WHITE);
        }
        else
        {
            display.drawBitmap(92, 26, Press_square_bitmap, 16, 12, SSD1306_WHITE);
        }
        // L1・L2・R1・R2の配置
        if (SW_left1)
        {
            display.drawBitmap(2, 2, Normal_L1key_bitmap, 16, 12, SSD1306_WHITE);
        }
        else
        {
            display.drawBitmap(2, 2, Press_L1key_bitmap, 16, 12, SSD1306_WHITE);
        }
        if (SW_left2)
        {
            display.drawBitmap(22, 2, Normal_L2key_bitmap, 16, 12, SSD1306_WHITE);
        }
        else
        {
            display.drawBitmap(22, 2, Press_L2key_bitmap, 16, 12, SSD1306_WHITE);
        }
        if (SW_right1)
        {
            display.drawBitmap(110, 2, Normal_R1key_bitmap, 16, 12, SSD1306_WHITE);
        }
        else
        {
            display.drawBitmap(110, 2, Press_R1key_bitmap, 16, 12, SSD1306_WHITE);
        }
        if (SW_right2)
        {
            display.drawBitmap(90, 2, Normal_R2key_bitmap, 16, 12, SSD1306_WHITE);
        }
        else
        {
            display.drawBitmap(90, 2, Press_R2key_bitmap, 16, 12, SSD1306_WHITE);
        }
        // ジョイコンの配置
        int Board_lx = int(left_X / 20 + 47);
        int Board_ly = int(left_Y / 20 + 43);
        int Board_rx = int(right_X / 20 + 74);
        int Board_ry = int(right_Y / 20 + 43);
        display.fillRect(Board_lx, Board_ly, 7, 7, SSD1306_WHITE);
        display.fillRect(Board_rx, Board_ry, 7, 7, SSD1306_WHITE);
        // Serial.println(String(left_X) + " -> " + String(Board_lx));

        display.display(); // 表示
        delay(50);
        display.clearDisplay(); // クリア

        // R1、R2、L1、L2のいずれかが押されるとキックする
        CheckKick(SW_right1 + SW_right2 + SW_left1 + SW_left2);
    }
}

// App2_PS3Checkerを起動させる関数
void App2_PS3Checker()
{
    while (!SW_cross)
    {
        M5PICOget(); // M5-picoからの値を更新

        // ホーム画面の配置
        display.drawBitmap(0, 0, HomeView_PS3CheckerApp2_bitmap, 128, 64, SSD1306_WHITE);

        // AnalogStickの値を表示
        display.setTextSize(1);              // テキストサイズを設定
        display.setTextColor(SSD1306_BLACK); // 白で表示
        // 左のAnalogStick
        display.setCursor(19, 15);   // どこにプリントさせるか
        display.print(left_X);       // 値をプリント
        display.setCursor(19, 27);   // どこにプリントさせるか
        display.print(left_Y);       // 値をプリント
        display.setCursor(19, 39);   // どこにプリントさせるか
        display.print(LStick_Angle); // 値をプリント
        if (LStick_Angle != -1)
        {

            display.setCursor(19, 51);  // どこにプリントさせるか
            display.print(LStick_Dist); // 値をプリント
        }
        else
        {
            display.setCursor(19, 51); // どこにプリントさせるか
            display.print(-1.00);      // 値をプリント
        }
        // 右のAnalogStick
        display.setCursor(84, 15);   // どこにプリントさせるか
        display.print(right_X);      // 値をプリント
        display.setCursor(84, 27);   // どこにプリントさせるか
        display.print(right_Y);      // 値をプリント
        display.setCursor(84, 39);   // どこにプリントさせるか
        display.print(RStick_Angle); // 値をプリント
        if (RStick_Angle != -1)
        {
            display.setCursor(84, 51);  // どこにプリントさせるか
            display.print(RStick_Dist); // 値をプリント
        }
        else
        {
            display.setCursor(84, 51); // どこにプリントさせるか
            display.print(-1.00);      // 値をプリント
        }

        display.display(); // 表示
        delay(50);
        display.clearDisplay(); // クリア
    }
}