#pragma once

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "nameTag.hpp"
#include "timer.hpp"

bool uiInit(TwoWire *wire, uint8_t address, uint8_t width, uint8_t height);
void uiProcess(bool show, bool enterbtn, bool rightbtn, bool leftbtn);

// 描画
void uiDrawCircleMeter(bool show, int x0, int y0, int r, const char *s, int deg);
void uiPrintDebug(const char *msg);

//　データ取得
bool uiActionDecided();
bool uiModeDecided();
bool uiRunning();
int uiActionNumber();
int uiModeNumber();
int uiConfigNumber();
bool uiConfigActive(uint8_t index);