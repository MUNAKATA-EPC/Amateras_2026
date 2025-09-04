/*モーターやディスプレイの制御*/
#pragma once

#include "Kicker/Kicker.hpp"
#include "Motors/Motors.hpp"
#include "UI/UI.hpp"

UI ui(&Wire1, 0x3C, 128, 64);