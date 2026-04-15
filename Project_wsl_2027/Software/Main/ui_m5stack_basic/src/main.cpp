#include <Arduino.h>

// SPIFFS (内蔵ファイルシステム) を使うためのインクルード
#include <SPIFFS.h>

// LovyanGFX設定
#define LGFX_AUTODETECT
#include <LovyanGFX.hpp>
static LGFX tft;

// Button
#include "button.hpp"
Button btnA;
Button btnB;
Button btnC;

void setup()
{
    btnA.init(39, INPUT_PULLUP); // ボタンAはGPIO39に接続
    btnB.init(38, INPUT_PULLUP); // ボタンBはGPIO38に接続
    btnC.init(37, INPUT_PULLUP); // ボタンCはGPIO37に接続

    tft.init();
    tft.setRotation(1);
    tft.setBrightness(128);

    if (!SPIFFS.begin(true))
    {
        tft.println("SPIFFS Mount Failed!");
        return;
    }

    tft.fillScreen(TFT_BLACK);
    tft.drawJpgFile(SPIFFS, "/kitagawa1.jpg", 0, 0);
}

int count = 0, old_count = 0;

void loop()
{
    btnA.update();
    btnB.update();
    btnC.update();

    if (btnA.isReleased())
    {
        count = (count + 1) % 5;
    }
    if (btnB.isReleased())
    {
        count = (count - 1 + 5) % 5;
    }

    if (count != old_count)
    {
        String filename = "/kitagawa" + String(count) + ".jpg";
        if (SPIFFS.exists(filename))
        {
            tft.drawJpgFile(SPIFFS, filename.c_str(), 0, 0);
        }
        else
        {
            tft.fillScreen(TFT_BLACK);
            tft.setCursor(0, 0);
            tft.setTextColor(TFT_WHITE);
            tft.println("File not found:");
            tft.println(filename);
        }
    }

    old_count = count;
    delay(1);
}