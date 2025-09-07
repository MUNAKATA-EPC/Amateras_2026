#include "Drivers.hpp"

// 定義
Drivers::Drivers()
    : kicker(7, 8),
      ui(&Wire, 0x3C, 128, 64)
{
}

// 開始
void Drivers::begin()
{
    kicker.begin(700);
    ui.begin(&Button(11, INPUT_PULLDOWN), &Button(12, INPUT_PULLDOWN), &Button(10, INPUT_PULLDOWN));
}

// ショートカットキー
Drivers drivers;
Kicker &kicker = drivers.kicker;
UI &ui = drivers.ui;