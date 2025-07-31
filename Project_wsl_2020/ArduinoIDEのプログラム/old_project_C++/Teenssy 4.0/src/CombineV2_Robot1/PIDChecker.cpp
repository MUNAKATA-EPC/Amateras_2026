#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include "USE_LCD.h"
#include "USE_m5pico.h"
#include "USE_BNO.h"
#include "USE_L3GD20H.h"
#include "USE_PIDv2.h"
#include "USE_ControlSW.h"
#include "USE_NeoPixel.h"

// App1_PIDCheckerを起動させる関数
void App1_PIDChecker()
{
    while (!ControlSW_left && !ControlSW_front)
    {
        M5PICOget();    // M5-picoからの値を更新
        ControlSWget(); // コントロール基板のSWを更新

        display.drawBitmap(0, 0, NoImage_bitmap, 128, 64, SSD1306_WHITE);

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
    }
}

// App2_PIDCheckerを起動させる関数
void App2_PIDChecker()
{
    while (!ControlSW_left && !ControlSW_front)
    {
        M5PICOget();    // M5-picoからの値を更新
        ControlSWget(); // コントロール基板のSWを更新

        display.drawBitmap(0, 0, HomeView_PIDCheckerApp2_bitmap, 128, 64, SSD1306_WHITE);

        // L3GD20Hの値を表示
        display.setTextSize(1);              // テキストサイズを設定
        display.setTextColor(SSD1306_BLACK); // 白で表示
        display.setCursor(83, 19);           // どこにプリントさせるか
        int L3GDdeg = L3GD20Hread();
        if (L3GDdeg != -1)
        {
            display.print(L3GDdeg); // 値をプリント
        }
        else
        {
            display.print("Cal..."); // 文字をプリント
        }
        // L3GD20Hの矢印を作図
        if (L3GDdeg != -1)
        {
            int L3GD_x = -int(15 * sin(radians(L3GDdeg))) + 111;
            int L3GD_y = -int(15 * cos(radians(L3GDdeg))) + 46;
            display.drawLine(111, 46, L3GD_x, L3GD_y, WHITE);
        }

        // BNOの値を表示
        display.setTextSize(1);              // テキストサイズを設定
        display.setTextColor(SSD1306_BLACK); // 白で表示
        display.setCursor(19, 19);           // どこにプリントさせるか
        int BNOdeg = BNOread();
        display.print(BNOdeg); // 値をプリント
        // BNOの矢印を作図
        int BNO_x = -int(15 * sin(radians(BNOdeg))) + 16;
        int BNO_y = -int(15 * cos(radians(BNOdeg))) + 46;
        display.drawLine(16, 46, BNO_x, BNO_y, WHITE);

        // それぞれのゲインを表示
        display.setTextSize(1);              // テキストサイズを設定
        display.setTextColor(SSD1306_BLACK); // 白で表示
        display.setCursor(51, 30);           // どこにプリントさせるか
        display.print(Kp);                   // 値をプリント
        display.setCursor(51, 42);           // どこにプリントさせるか
        display.print(Ki);                   // 値をプリント
        display.setCursor(51, 54);           // どこにプリントさせるか
        display.print(Kd);                   // 値をプリント

        // NeoPixel・LCDを表示
        LCD_Show(); // LCDを表示

        LEDedge_NeoLight(abs(L3GDdeg - 360), 2, 240, 260, 100); // 右ステックの角度をNeoPixelで光らせる
        LEDedge_NeoLight(abs(BNOdeg - 360), 2, 0, 20, 100);     // 左ステックの角度をNeoPixelで光らせる

        NeoPixel_Show();  // NeoPixelを表示
        delay(40);        // 40ms待つ
        NeoPixel_Clear(); // NeoPixelをクリア

        LCD_Clear(); // LCDをクリア
    }
}