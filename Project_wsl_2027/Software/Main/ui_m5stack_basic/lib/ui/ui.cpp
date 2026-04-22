#include "ui.hpp"

static LGFX tft;
static LGFX_Sprite canvas(&tft);

const uint32_t black_color = 0x000000;
const uint32_t white_color = 0xFFFFFF;
const uint32_t gray_color = 0x7F7F7F;
const uint32_t red_color = 0xFF0000;
const uint32_t pink_color = 0xE74D4D;
const uint32_t blue_color = 0x4472C4;
const uint32_t yellow_color = 0xFFD966;
const uint32_t green_color = 0x70AD47;

static uiToggle runToggle(5, 5, 60, 95, gray_color, black_color, red_color, "on", white_color, blue_color, "off", white_color, false, 5);

const uint32_t goal_cycle_bg_colors[] = {yellow_color, blue_color, gray_color};
const char *goal_cycle_labels[] = {"YELLOW", "BLUE", "GYRO"};
const uint32_t goal_cycle_txt_colors[] = {white_color, white_color, white_color};
static uiCycleButton goalCycleBtn(5, 105, 60, 80, 3, goal_cycle_bg_colors, goal_cycle_labels, goal_cycle_txt_colors);

static uiToggle showToggle(255, 5, 60, 95, gray_color, black_color, red_color, "show", white_color, blue_color, "hide", white_color, false, 5);

const uint32_t pos_cycle_bg_colors[] = {green_color, gray_color};
const char *pos_cycle_labels[] = {"POS", "ODA"};
const uint32_t pos_cycle_txt_colors[] = {white_color, white_color};
static uiCycleButton posCycleBtn(255, 105, 60, 80, 2, pos_cycle_bg_colors, pos_cycle_labels, pos_cycle_txt_colors);

static uiButton atkBtn(70, 5, 180, 180, pink_color, "atk", 0xFFFFFF);

static uiButton monitorBtn(70, 190, 180, 45, gray_color, "monitor", 0xFFFFFF);
static uiButton leftArrowBtn(5, 190, 60, 45, gray_color, "<-", 0xFFFFFF);
static uiButton rightArrowBtn(255, 190, 60, 45, gray_color, "->", 0xFFFFFF);

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
    // 描画クリア
    canvas.fillScreen(TFT_WHITE);

    // コントロールオブジェクト更新
    runToggle.update(mouse);
    goalCycleBtn.update(mouse);
    showToggle.update(mouse);
    posCycleBtn.update(mouse);
    atkBtn.update(mouse);
    monitorBtn.update(mouse);
    leftArrowBtn.update(mouse);
    rightArrowBtn.update(mouse);

    // コントロールオブジェクト描画
    runToggle.draw(canvas);
    goalCycleBtn.draw(canvas);
    showToggle.draw(canvas);
    posCycleBtn.draw(canvas);
    atkBtn.draw(canvas);
    monitorBtn.draw(canvas);
    leftArrowBtn.draw(canvas);
    rightArrowBtn.draw(canvas);

    // マウス描画
    canvas.fillCircle(mouse.posi.x, mouse.posi.y, 3, TFT_BLACK);

    // 描画更新
    canvas.pushSprite(0, 0);
}

int uiWidth() { return tft.width(); }
int uiHeight() { return tft.height(); }