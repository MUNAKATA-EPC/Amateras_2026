#pragma once

// common類
#include "angleHelper.hpp"
#include "gaussianShifter.hpp"
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
#include "uss.hpp"

void playJAMVer2(JAM::Mode mode);
void printJAMVer2(JAM::Mode mode);