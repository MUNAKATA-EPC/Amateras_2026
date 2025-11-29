#pragma once

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "uiNameTag.hpp"
#include "timer.hpp"
// sensor類
#include "bno.hpp"
#include "button.hpp"
#include "digitalSensor.hpp"
#include "ir.hpp"
#include "line.hpp"
#include "ps3.hpp"
#include "openmv.hpp"

bool uiInit(TwoWire *wire, uint8_t address, uint8_t width, uint8_t height);

// ssdの初期化・表示
void uiShow();
void uiClear();

// 描画
void uiDrawCircleMeter(int x0, int y0, int r, const char *s, int deg);
void uiButtonUpdate(bool enterbtn, bool backbtn, bool rightbtn, bool leftbtn);
void uiDrawMain();
void uiPrint(int x0, int y0, String msg);
void uiPrintDebug(const char *msg);

// configの内容をプリントする関数
// config_num:書きたい行(1~CONFIG_NUM_LIMIT(4) - 1), msg:書きたいメッセージ, default_num:defaultの数字, min_num・max_num:範囲
// 出力は計算した値
int uiConfigPrintAndGet(int config_num, String msg, int default_num, int min_num, int max_num);

// 　データ取得
int uiMeterNumber(); // 0~7を周期する
bool uiActionDecided();
bool uiModeDecided();
bool uiRunning();
int uiActionNumber();
int uiModeNumber();
int uiConfigNumber();
bool uiConfigData(uint8_t index);