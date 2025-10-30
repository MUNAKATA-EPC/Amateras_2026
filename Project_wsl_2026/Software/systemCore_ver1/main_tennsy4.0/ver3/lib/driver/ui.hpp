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
void uiDrawMain(bool enterbtn, bool rightbtn, bool leftbtn);
void uiPrint(int x0, int y0, String msg);
void uiPrintDebug(const char *msg);

// 　データ取得
int uiMeterNumber(); // 0~7を周期する
bool uiActionDecided();
bool uiModeDecided();
bool uiRunning();
int uiActionNumber();
int uiModeNumber();
int uiConfigNumber();
bool uiConfigActive(uint8_t index);