#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include "USE_LCD.h"
#include "USE_m5pico.h"

// App1_PlaySoccerを起動させる関数
void App1_PlaySoccer()
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

// App2_PlaySoccerを起動させる関数
void App2_PlaySoccer()
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