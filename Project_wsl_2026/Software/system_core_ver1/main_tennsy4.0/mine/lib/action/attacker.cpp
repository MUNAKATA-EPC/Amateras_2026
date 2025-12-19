#include "attacker.hpp"

void attackWithGyro();
void attackWithCam(bool attack_goal_detected, int attack_goal_deg, int attack_goal_dis, bool defence_goal_detected, int defence_goal_deg, int defence_goal_dis);

void playAttacker(Attacker::Mode mode)
{
    if (mode == Attacker::Mode::GYRO)
    {
        attackWithGyro();
    }
    else
    {
        if (mode == Attacker::Mode::YELLOWGOAL)
        {
            attackWithCam(yellowGoalDetected(), yellowGoalDeg(), yellowGoalDis(), blueGoalDetected(), blueGoalDeg(), blueGoalDis());
        }
        else
        {
            attackWithCam(blueGoalDetected(), blueGoalDeg(), blueGoalDis(), yellowGoalDetected(), yellowGoalDeg(), yellowGoalDis());
        }
    }
}

static Timer line_timer;
static Timer kick_timer;
static bool old_catch = false;

void attackWithGyro() // ジャイロで攻撃
{
    motorsPdProcess(&pd_gyro, bnoDeg(), 0); // ジャイロで姿勢制御

    // キッカー
    bool now_catch = (catchSensor.read() == HIGH);

    if (now_catch == true && old_catch == false)
    {
        kick_timer.reset();
    }

    if (now_catch && kick_timer.everReset() && kick_timer.msTime() >= 100UL)
    {
        kicker1.kick(true);
    }
    else
    {
        kicker1.kick(false);
    }

    old_catch = now_catch;
    // キッカー

    const int motor_line_max_power = 80;
    const int motor_ir_max_power = 95;

    if (line_timer.everReset() && line_timer.msTime() < 100UL)
    {
        if (fieldDetected())
        {
            motorsMove(fieldDeg(), motor_line_max_power);
        }
        else
        {
            motorsMove(lineRingFirstDeg() + 180, motor_line_max_power);
        }
    }
    else if (lineRingDetected())
    {
        if (fieldDetected())
        {
            motorsMove(fieldDeg(), motor_line_max_power);
        }
        else
        {
            motorsMove(lineRingFirstDeg() + 180, motor_line_max_power);
        }

        line_timer.reset();
    }
    else if (irDetected())
    {
        if (irDeg() >= -11 && irDeg() <= 9)
        {
            motorsMove(0, motor_ir_max_power);
        }
        else
        {
            if (irDis() >= 60.0f)
            {
                motorsMove(irDeg(), motor_ir_max_power);
            }
            else
            {
                int diff = (int)roundf(irVal() * 1.11f);

                if (irDeg() > 0) // 左にいるとき
                {
                    if (irDeg() <= 45)
                    {
                        motorsMove(irDeg() + diff, motor_ir_max_power * 0.65f);
                    }
                    else if (irDeg() <= 160)
                    {
                        motorsMove(irDeg() + diff, motor_ir_max_power);
                    }
                    else
                    {
                        motorsMove(irDeg() + diff, motor_ir_max_power);
                    }
                }
                else // 右にいるとき
                {
                    if (irDeg() >= -45)
                    {
                        motorsMove(irDeg() - diff, motor_ir_max_power * 0.65f);
                    }
                    else if (irDeg() >= -160)
                    {
                        motorsMove(irDeg() - diff, motor_ir_max_power);
                    }
                    else
                    {
                        motorsMove(irDeg() - diff, motor_ir_max_power);
                    }
                }
            }
        }
    }
    else
    {
        motorsPdMove();
    }
}

Timer cam_pd_timer;

void attackWithCam(bool attack_goal_detected, int attack_goal_deg, int attack_goal_dis, bool defence_goal_detected, int defence_goal_deg, int defence_goal_dis) // カメラで攻撃
{
    if (attack_goal_detected && attack_goal_dis < 110)
    {
        motorsPdProcess(&pd_cam, attack_goal_deg, 0); // カメラで姿勢制御
    }
    else
    {
        motorsPdProcess(&pd_gyro, bnoDeg(), 0); // ジャイロで姿勢制御
    }

    if (catchSensor.read() == HIGH && attack_goal_detected && abs(attack_goal_deg) < 70)
    {
        kicker1.kick(true);
    }
    else
    {
        kicker1.kick(false);
    }

    const int motor_line_max_power = 80;
    const int motor_ir_max_power = 95;

    if (line_timer.everReset() && line_timer.msTime() < 100UL)
    {
        if (fieldDetected())
        {
            motorsMove(fieldDeg(), motor_line_max_power);
        }
        else
        {
            motorsMove(lineRingFirstDeg() + 180, motor_line_max_power);
        }
    }
    else if (lineRingDetected())
    {
        if (fieldDetected())
        {
            motorsMove(fieldDeg(), motor_line_max_power);
        }
        else
        {
            motorsMove(lineRingFirstDeg() + 180, motor_line_max_power);
        }

        line_timer.reset();
    }
    else if (irDetected())
    {
        if (irDeg() >= -11 && irDeg() <= 9)
        {
            motorsMove(0, motor_ir_max_power);
        }
        else if (irDeg() >= -16 && irDeg() <= 16)
        {
            motorsMove(irDeg() * 1.11f, motor_ir_max_power);
        }
        else
        {
            if (irDis() >= 60.0f)
            {
                motorsMove(irDeg(), motor_ir_max_power);
            }
            else
            {
                int diff = (int)roundf(irVal() * 1.11f);

                if (irDeg() > 0) // 左にいるとき
                {
                    if (irDeg() <= 45)
                    {
                        motorsMove(irDeg() + diff, motor_ir_max_power * 0.65f);
                    }
                    else if (irDeg() <= 160)
                    {
                        motorsMove(irDeg() + diff, motor_ir_max_power);
                    }
                    else
                    {
                        motorsMove(irDeg() + diff, motor_ir_max_power);
                    }
                }
                else // 右にいるとき
                {
                    if (irDeg() >= -45)
                    {
                        motorsMove(irDeg() - diff, motor_ir_max_power * 0.65f);
                    }
                    else if (irDeg() >= -160)
                    {
                        motorsMove(irDeg() - diff, motor_ir_max_power);
                    }
                    else
                    {
                        motorsMove(irDeg() - diff, motor_ir_max_power);
                    }
                }
            }
        }
    }
    else
    {
        motorsPdMove();
    }
}