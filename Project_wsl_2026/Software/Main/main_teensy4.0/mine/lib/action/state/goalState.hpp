#pragma once

// common類
#include "angleHelper.hpp"
#include "pd.hpp"
#include "timer.hpp"
#include "vector.hpp"
#include "movementAverage.hpp"
#include "smoothHelper.hpp"
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

// 自分ゴールと相手ゴールの確認
void goalPositionCheck(Attacker::Mode mode);
void goalPositionCheck(Defender::Mode mode);

bool attackGoalDetected();
float attackGoalDeg();
float attackGoalDis();

bool defenceGoalDetected();
float defenceGoalDeg();
float defenceGoalDis();

// 自分ゴールの白線上にいるか ___(ゴール)___
bool isMyDefenceArea();

// 相手ゴールの白線上にいるか ___(ゴール)___
bool isMyAttackArea();