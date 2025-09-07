/*モーターやディスプレイの制御*/
#pragma once

#include "Sensors.hpp"
#include "Kicker/Kicker.hpp"
#include "Motors/Motors.hpp"
#include "UI/UI.hpp"

class Drivers
{
public:
    Kicker kicker;
    UI ui;

    Drivers();
    void begin();
};

// グローバル参照（宣言のみ）
extern Drivers drivers;
extern Kicker &kicker;
extern UI &ui;