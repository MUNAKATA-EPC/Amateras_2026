/*モーターやディスプレイの制御*/
#pragma once

#include "Sensors.hpp"
#include "Kicker/Kicker.hpp"
#include "Motors/Motors.hpp"
#include "UI/UI.hpp"

extern Kicker kicker;
extern UI ui;
extern Motors motors;

// すべてのドライバーを開始させる関数
void DriversBegin();