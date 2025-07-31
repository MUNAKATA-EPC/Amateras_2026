#ifndef USE_LCD_H

#define USE_LCD_H

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
// displayを定義
extern Adafruit_SSD1306 display;

// 最初のロゴ表示にかける時間
extern const int LCD_ShowLogo_ms;

// Menu画面
extern const unsigned char Menu_bitmap[] PROGMEM;
// アプリの詳細選択アイコン
extern const unsigned char Press_app1_bitmap[] PROGMEM;
extern const unsigned char Press_app2_bitmap[] PROGMEM;
extern const unsigned char NoPress_app1_bitmap[] PROGMEM;
extern const unsigned char NoPress_app2_bitmap[] PROGMEM;
extern const unsigned char App_SelectStick_bitmap[] PROGMEM;
// それぞれのアプリのアイコン
extern const unsigned char Icon_MainProgram_bitmap[] PROGMEM;
extern const unsigned char Icon_PS3Checker_bitmap[] PROGMEM;
extern const unsigned char Icon_PIDChecker_bitmap[] PROGMEM;

// 操作のための矢印
// 普通のスクリプト
extern const unsigned char Normal_LeftArrow_bitmap[] PROGMEM;
extern const unsigned char Normal_RightArrow_bitmap[] PROGMEM;
extern const unsigned char Normal_DownArrow_bitmap[] PROGMEM;
extern const unsigned char Normal_UpArrow_bitmap[] PROGMEM;
// 押されたスクリプト
extern const unsigned char Press_LeftArrow_bitmap[] PROGMEM;
extern const unsigned char Press_RightArrow_bitmap[] PROGMEM;
extern const unsigned char Press_DownArrow_bitmap[] PROGMEM;
extern const unsigned char Press_UpArrow_bitmap[] PROGMEM;

// PS3Checkerで使う画像のデータ
// PS3Checkerで使う画像
// App1のホーム画面
extern const unsigned char HomeView_PS3CheckerApp1_bitmap[] PROGMEM;
// App2のホーム画面
extern const unsigned char HomeView_PS3CheckerApp2_bitmap[] PROGMEM;
// ボタンが押されてない時の表示
extern const unsigned char Normal_triangle_bitmap[] PROGMEM;
extern const unsigned char Normal_circle_bitmap[] PROGMEM;
extern const unsigned char Normal_cross_bitmap[] PROGMEM;
extern const unsigned char Normal_square_bitmap[] PROGMEM;
extern const unsigned char Normal_0cross_bitmap[] PROGMEM;
extern const unsigned char Normal_90cross_bitmap[] PROGMEM;
extern const unsigned char Normal_180cross_bitmap[] PROGMEM;
extern const unsigned char Normal_270cross_bitmap[] PROGMEM;
extern const unsigned char Normal_L1key_bitmap[] PROGMEM;
extern const unsigned char Normal_L2key_bitmap[] PROGMEM;
extern const unsigned char Normal_R1key_bitmap[] PROGMEM;
extern const unsigned char Normal_R2key_bitmap[] PROGMEM;
// ボタンが押されたときの表示
extern const unsigned char Press_triangle_bitmap[] PROGMEM;
extern const unsigned char Press_circle_bitmap[] PROGMEM;
extern const unsigned char Press_cross_bitmap[] PROGMEM;
extern const unsigned char Press_square_bitmap[] PROGMEM;
extern const unsigned char Press_0cross_bitmap[] PROGMEM;
extern const unsigned char Press_90cross_bitmap[] PROGMEM;
extern const unsigned char Press_180cross_bitmap[] PROGMEM;
extern const unsigned char Press_270cross_bitmap[] PROGMEM;
extern const unsigned char Press_L1key_bitmap[] PROGMEM;
extern const unsigned char Press_L2key_bitmap[] PROGMEM;
extern const unsigned char Press_R1key_bitmap[] PROGMEM;
extern const unsigned char Press_R2key_bitmap[] PROGMEM;

// PIDCheckerで使う画像データ
// App2のホーム画面
extern const unsigned char HomeView_PIDCheckerApp2_bitmap[] PROGMEM;

// PlaySoccerで使うデータ
// App1(BNO)のホーム画面
extern const unsigned char HomeView_PlaySoccerApp1_bitmap[] PROGMEM;
// 再生ボタン
extern const unsigned char PlayStart_bitmap[] PROGMEM;
extern const unsigned char PlayStop_bitmap[] PROGMEM;
extern const unsigned char KickStart_bitmap[] PROGMEM;
extern const unsigned char KickStop_bitmap[] PROGMEM;

// NoImageの画像データ
extern const unsigned char NoImage_bitmap[] PROGMEM;

// I2Cのアドレスの設定を行う関数
void LCDbegin(uint8_t adress);

// どのプログラムを走らせるか決定する関数  0と1:MainProgram, 2と3:PIDChecker, 4と5:PS3Checker
int SelectMenu();

// PlaySoccerのプログラムを動かす関数_App1
void App1_PlaySoccer();
// PlaySoccerのプログラムを動かす関数_App2
void App2_PlaySoccer();
// PS3Checkerのプログラムを動かす関数_App1
void App1_PS3Checker();
// PS3Checkerのプログラムを動かす関数_App2
void App2_PS3Checker();
// PIDCheckerのプログラムを動かす関数_App1
void App1_PIDChecker();
// PIDCheckerのプログラムを動かす関数_App2
void App2_PIDChecker();

#endif