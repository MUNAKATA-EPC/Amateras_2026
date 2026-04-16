#include "ui.hpp"

static LGFX tft;
static LGFX_Sprite canvas(&tft);

bool uiInit()
{
    tft.init();
    tft.setRotation(1);
    tft.setBrightness(128);

    if (!SPIFFS.begin(true))
    {
        tft.println("SPIFFS Mount Failed!");
        return false;
    }

    tft.fillScreen(TFT_BLACK);

    canvas.setColorDepth(8);
    canvas.createSprite(tft.width(), tft.height());

    return true;
}

void uiUpdate(mouse_t mouse)
{
    mouse_t current_mouse = mouse;

    // 描画クリア
    canvas.fillScreen(TFT_BLACK);

    // マウス描画
    canvas.fillCircle(mouse.posi.x, mouse.posi.y, 3, TFT_WHITE);

    canvas.drawPngFile(SPIFFS, "/icon.png", mouse.posi.x - 16, mouse.posi.y - 16);

    // 描画更新
    canvas.pushSprite(0, 0);
}

int uiWidth() { return tft.width(); }
int uiHeight() { return tft.height(); }