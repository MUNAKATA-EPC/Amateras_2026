#pragma once

#include <Arduino.h>

// posi
#include "posi.hpp"

// SPIFFS (内蔵ファイルシステム) を使うためのインクルード
#include <SPIFFS.h>

// LovyanGFX設定
#define LGFX_AUTODETECT
#include <LovyanGFX.hpp>

bool uiInit();

struct mouse_t
{
    posi_t posi;
    bool pushing;
};
void uiUpdate(mouse_t mouse);

int uiWidth();
int uiHeight();