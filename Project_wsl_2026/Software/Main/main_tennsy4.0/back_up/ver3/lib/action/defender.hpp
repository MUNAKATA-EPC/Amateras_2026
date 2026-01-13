#pragma once

// action類
#include "attacker.hpp"

// common類
#include "angle_helper.hpp"
#include "pd.hpp"
#include "timer.hpp"
#include "vector.hpp"
// driver類
#include "dribbler.hpp"
#include "kicker.hpp"
#include "motors.hpp"
#include "ui.hpp"
// sensor類
#include "bno.hpp"
#include "button.hpp"
#include "digitalSensor.hpp"
#include "ir.hpp"
#include "line.hpp"
#include "ps3.hpp"
#include "openmv.hpp"

void playDefender(Defender::Mode mode);