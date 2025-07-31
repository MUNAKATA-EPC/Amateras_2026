#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include "USE_LCD.h"
#include "USE_m5pico.h"
#include "USE_BNO.h"

// App1_PIDCheckerを起動させる関数
void App1_PIDChecker()
{
    while (!SW_cross)
    {
        M5PICOget(); // M5-picoからの値を更新

        display.drawBitmap(0, 0, NoImage_bitmap, 128, 64, SSD1306_WHITE);

        display.display(); // 表示
        delay(50);
        display.clearDisplay(); // クリア
    }
}

// App2_PIDCheckerを起動させる関数
void App2_PIDChecker()
{
    while (!SW_cross)
    {
        M5PICOget(); // M5-picoからの値を更新

        display.drawBitmap(0, 0, HomeView_PIDCheckerApp2_bitmap, 128, 64, SSD1306_WHITE);

        // BNOの値を表示
        display.setTextSize(1);              // テキストサイズを設定
        display.setTextColor(SSD1306_BLACK); // 白で表示
        display.setCursor(19, 19);           // どこにプリントさせるか
        display.print(BNOread());            // 値をプリント

        // BNOの矢印を作図
        int BNO_x = - int(15 * sin(radians(BNOread()))) + 16;
        int BNO_y = - int(15 * cos(radians(BNOread()))) + 46;
        display.drawLine(16, 46, BNO_x, BNO_y, WHITE);

        display.display(); // 表示
        delay(50);
        display.clearDisplay(); // クリア
    }
}