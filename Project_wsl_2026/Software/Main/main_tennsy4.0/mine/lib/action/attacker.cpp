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

    const int motor_line_max_power = 75;
    const int motor_ir_max_power = 75;

    if (lineRingDetected()) // エンジェルライン
    {
        motorsMove(fieldDeg(), motor_line_max_power);

        line_timer.reset();
    }
    else if (irDetected())
    {
        if (irDeg() >= -8 && irDeg() <= 8)
        {
            motorsMove(0, motor_ir_max_power);
        }
        else if (irDeg() >= -10 && irDeg() <= 10)
        {
            motorsMove(irDeg() * 1.11f, motor_ir_max_power);
        }
        else
        {
            if (irDis() >= 53.0f)
            {
                motorsMove(irDeg(), motor_ir_max_power);
            }
            else
            {
                int diff = 50;

                if (irDeg() > 0) // 左にいるとき
                {
                    if (irDeg() <= 45)
                    {
                        motorsMove(irDeg() + diff, motor_ir_max_power);
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
                        motorsMove(irDeg() - diff, motor_ir_max_power);
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

static Timer cam_pd_timer;

void attackWithCam(bool attack_goal_detected, int attack_goal_deg, int attack_goal_dis, bool defence_goal_detected, int defence_goal_deg, int defence_goal_dis) // カメラで攻撃
{
    if (attack_goal_detected && attack_goal_dis <= 110)
    {
        motorsPdProcess(&pd_cam, attack_goal_deg, 0); // カメラで姿勢制御
    }
    else
    {
        motorsPdProcess(&pd_gyro, bnoDeg(), 0); // ジャイロで姿勢制御
    }

    if (catchSensor.read() == HIGH && attack_goal_detected && abs(attack_goal_deg) <= 70)
    {
        kicker1.kick(true);
    }
    else
    {
        kicker1.kick(false);
    }

    const int motor_line_max_power = 50;
    const int motor_ir_max_power = 90;

    if (attack_goal_detected && attack_goal_dis < 70)
    {
        motorsMove(attack_goal_deg + 180, motor_line_max_power);
    }
    else if (defence_goal_detected && defence_goal_dis < 70)
    {
        motorsMove(defence_goal_deg + 180, motor_line_max_power);
    }
    else if (line_timer.everReset() && line_timer.msTime() < 50UL)
    {
        motorsMove(fieldDeg(), motor_line_max_power);
    }
    else if (lineRingDetected()) // エンジェルライン
    {
        motorsMove(fieldDeg(), motor_line_max_power);
        line_timer.reset();
    }
    /*else if (ussRightDetected() && ussLeftDetected() && ussRightDis() < 35 && (ussRightDis() + ussLeftDis()) > 80)
    {
        if (irDetected() && irDeg() <= 0)
        {
            if (irDeg() > -75)
            {
                motorsMove(0, motor_ir_max_power);
            }
            else
            {
                motorsMove(180, motor_ir_max_power);
            }
        }
        else
        {
            motorsMove(90, motor_ir_max_power);
        }
    }
    else if (ussRightDetected() && ussLeftDetected() && ussLeftDis() < 35 && (ussRightDis() + ussLeftDis()) > 80)
    {
        if (irDetected() && irDeg() > 0)
        {
            if (irDeg() < 75)
            {
                motorsMove(0, motor_ir_max_power);
            }
            else
            {
                motorsMove(180, motor_ir_max_power);
            }
        }
        else
        {
            motorsMove(-90, motor_ir_max_power);
        }
    }*/
    else if (irDetected())
    {
        Vector mawarikomi_vec;

        if (irDis() >= 666.0f)
        {
            motorsMove(irDeg(), motor_ir_max_power);
        }
        else if (irDeg() >= -10 && irDeg() <= 10)
        {
            motorsMove(0, motor_ir_max_power);
        }
        else if (irDeg() >= -20 && irDeg() <= 20)
        {
            int diff = (irDeg() > 0) ? 27 : -27;
            motorsMove(irDeg() + diff, motor_ir_max_power);
        }
        else if (irDeg() >= -30 && irDeg() <= 30)
        {
            int diff = (irDeg() > 0) ? 40 : -40;
            motorsMove(irDeg() + diff, motor_ir_max_power);
        }
        else if (irDeg() >= -55 && irDeg() <= 55)
        {
            int diff = (irDeg() > 0) ? 45 : -45;
            motorsMove(irDeg() + diff, motor_ir_max_power);
        }
        else if (irDeg() >= -125 && irDeg() <= 125)
        {
            int diff = (irDeg() > 0) ? 55 : -55;
            motorsMove(irDeg() + diff, motor_ir_max_power);
        }
        else
        {
            int diff = (irDeg() > 0) ? 50 : -50;
            motorsMove(irDeg() + diff, motor_ir_max_power);
        }
    }
    else
    {
        motorsMove(fieldDeg(), 50);
    }
}