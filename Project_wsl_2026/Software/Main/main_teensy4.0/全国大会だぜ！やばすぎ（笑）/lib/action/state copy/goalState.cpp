#include "goalState.hpp"

// 攻撃ゴールの位置
static bool attack_goal_detected;
static float attack_goal_deg;
static float attack_goal_dis;
// 守備ゴールの位置
static bool defence_goal_detected;
static float defence_goal_deg;
static float defence_goal_dis;

void goalPositionCheckAtMode(Attacker::Mode mode)
{
    if (mode == Attacker::Mode::YELLOWGOAL)
    {
        attack_goal_detected = yellowGoalDetected();
        attack_goal_deg = yellowGoalDeg();
        attack_goal_dis = yellowGoalDis();

        defence_goal_detected = blueGoalDetected();
        defence_goal_deg = blueGoalDeg();
        defence_goal_dis = blueGoalDis();
    }
    else // if (mode == Attacker::Mode::BLUEGOAL)
    {
        defence_goal_detected = yellowGoalDetected();
        defence_goal_deg = yellowGoalDeg();
        defence_goal_dis = yellowGoalDis();

        attack_goal_detected = blueGoalDetected();
        attack_goal_deg = blueGoalDeg();
        attack_goal_dis = blueGoalDis();
    }
}

void goalPositionCheckDfMode(Defender::Mode mode)
{
    if (mode == Defender::Mode::YELLOWGOAL)
    {
        attack_goal_detected = yellowGoalDetected();
        attack_goal_deg = yellowGoalDeg();
        attack_goal_dis = yellowGoalDis();

        defence_goal_detected = blueGoalDetected();
        defence_goal_deg = blueGoalDeg();
        defence_goal_dis = blueGoalDis();
    }
    else // if (mode == Defender::Mode::BLUEGOAL)
    {
        defence_goal_detected = yellowGoalDetected();
        defence_goal_deg = yellowGoalDeg();
        defence_goal_dis = yellowGoalDis();

        attack_goal_detected = blueGoalDetected();
        attack_goal_deg = blueGoalDeg();
        attack_goal_dis = blueGoalDis();
    }
}

bool attackGoalDetected() { return attack_goal_detected; }
float attackGoalDeg() { return attack_goal_deg; }
float attackGoalDis() { return attack_goal_dis; }

bool defenceGoalDetected() { return defence_goal_detected; }
float defenceGoalDeg() { return defence_goal_deg; }
float defenceGoalDis() { return defence_goal_dis; }

bool isMyDefenceArea()
{
    return fabsf(fieldDeg()) < 90 &&
           defence_goal_detected &&
           fabsf(defence_goal_deg) > 90 &&
           defence_goal_dis < 86;
}

bool isMyAttackArea() { return fabsf(fieldDeg()) > 90 &&
                               attack_goal_detected &&
                               fabsf(attack_goal_deg) < 90 &&
                               attack_goal_dis < 86; }