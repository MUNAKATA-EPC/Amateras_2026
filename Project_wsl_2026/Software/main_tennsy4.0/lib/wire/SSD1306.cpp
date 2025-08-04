#include "SSD1306.hpp"

/*SSD1306をI2Cで制御する*/

Adafruit_SSD1306 *display = nullptr; // とりあえず定義

int display_width = 128; // displayの横幅格納用
int display_height = 64; // displayの縦幅格納用

void SSD1306_init(TwoWire *wire, uint8_t adress, int width, int height)
{
    display_width = width;
    display_height = height;

    /*if (display != nullptr)
    {
        delete display;
    }*/
    display = new Adafruit_SSD1306(display_width, display_height, wire, -1); // 新しく作る

    Timer my_display_timer;
    my_display_timer.reset(); // タイマーをリセット
    while (!(*display).begin(SSD1306_SWITCHCAPVCC, adress) && my_display_timer.get_time() < 5000)
    {
        delay(10); // 10ms待機
    } // SSD1306との通信が成功するか5000ms経つかそれまで待つ

    // 画面初期化
    (*display).clearDisplay();
}

void SSD1306_clear()
{
    // 画面初期化
    (*display).clearDisplay();
}

void SSD1306_show()
{
    // 画面表示
    (*display).display();
}

void SSD1306_write(int size, double ahead_x, double ahead_y, String Sentence, bool black)
{
    // テキスト表示
    (*display).setTextSize(size);
    if (!black)
    {
        (*display).setTextColor(SSD1306_WHITE, SSD1306_BLACK);
    }
    else
    {
        (*display).setTextColor(SSD1306_BLACK, SSD1306_WHITE);
    }
    (*display).setCursor(int(ahead_x), int(ahead_y));
    (*display).println(Sentence);
}

void SSD1306_bitmap(double x, double y, const unsigned char *bitmap, int width, int height, bool black)
{
    // ビットマップを表示
    if (!black)
    {
        (*display).drawBitmap(int(x), int(y), bitmap, width, height, SSD1306_WHITE);
    }
    else
    {
        (*display).drawBitmap(int(x), int(y), bitmap, width, height, SSD1306_BLACK);
    }
}

void SSD1306_line(double start_x, double start_y, double end_x, double end_y, bool black)
{
    // ラインを描画
    if (!black)
    {
        (*display).drawLine(int(start_x), int(start_y), int(end_x), int(end_y), SSD1306_WHITE);
    }
    else
    {
        (*display).drawLine(int(start_x), int(start_y), int(end_x), int(end_y), SSD1306_BLACK);
    }
}

void SSD1306_function_line(double origin_x, double origin_y, double a, double b, bool black)
{
    // 計算
    double start_x = 0;                                         // x = 0の時について
    double start_y = a * start_x - a * origin_x + b + origin_y; // x = 0の時のyを求める
    start_y = -start_y + display_height - 1;                    // 縦方向が正になるように調整
    double end_x = display_width - 1;                           // x = SSD1306_WIDTH - 1の時について
    double end_y = a * end_x - a * origin_x + b + origin_y;     // x = SSD1306_WIDTH - 1の時のyを求める
    end_y = -end_y + display_height - 1;                        // 縦方向が正になるように調整
    // ラインを描画
    if (!black)
    {
        (*display).drawLine(int(start_x), int(start_y), int(end_x), int(end_y), SSD1306_WHITE);
    }
    else
    {
        (*display).drawLine(int(start_x), int(start_y), int(end_x), int(end_y), SSD1306_BLACK);
    }
}

void SSD1306_circle(double point_x, double point_y, double circle_r, bool fill, bool black)
{
    // 円を描画
    if (!black)
    {
        if (fill)
        {
            (*display).fillCircle(int(point_x), int(point_y), int(circle_r), SSD1306_WHITE);
        }
        else
        {
            (*display).drawCircle(int(point_x), int(point_y), int(circle_r), SSD1306_WHITE);
        }
    }
    else
    {
        if (fill)
        {
            (*display).fillCircle(int(point_x), int(point_y), int(circle_r), SSD1306_BLACK);
        }
        else
        {
            (*display).drawCircle(int(point_x), int(point_y), int(circle_r), SSD1306_BLACK);
        }
    }
}

void SSD1306_rect(double topleft_x, double topleft_y, double width, double height, double corner_r, bool fill, bool black)
{
    // 矩形を描画
    if (!black)
    {
        if (fill)
        {
            (*display).fillRoundRect(int(topleft_x), int(topleft_y), int(width), int(height), int(corner_r), SSD1306_WHITE);
        }
        else
        {
            (*display).drawRoundRect(int(topleft_x), int(topleft_y), int(width), int(height), int(corner_r), SSD1306_WHITE);
        }
    }
    else
    {
        if (fill)
        {
            (*display).fillRoundRect(int(topleft_x), int(topleft_y), int(width), int(height), int(corner_r), SSD1306_BLACK);
        }
        else
        {
            (*display).drawRoundRect(int(topleft_x), int(topleft_y), int(width), int(height), int(corner_r), SSD1306_BLACK);
        }
    }
}

void SSD1306_triangle(double point1_x, double point1_y, double point2_x, double point2_y, double point3_x, double point3_y, bool fill, bool black)
{
    // 三角形を描画
    if (!black)
    {
        if (fill)
        {
            (*display).fillTriangle(int(point1_x), int(point1_y), int(point2_x), int(point2_y), int(point3_x), int(point3_y), SSD1306_WHITE);
        }
        else
        {
            (*display).drawTriangle(int(point1_x), int(point1_y), int(point2_x), int(point2_y), int(point3_x), int(point3_y), SSD1306_WHITE);
        }
    }
    else
    {
        if (fill)
        {
            (*display).fillTriangle(int(point1_x), int(point1_y), int(point2_x), int(point2_y), int(point3_x), int(point3_y), SSD1306_BLACK);
        }
        else
        {
            (*display).drawTriangle(int(point1_x), int(point1_y), int(point2_x), int(point2_y), int(point3_x), int(point3_y), SSD1306_BLACK);
        }
    }
}