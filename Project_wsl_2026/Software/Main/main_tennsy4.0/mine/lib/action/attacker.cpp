#include "attacker.hpp"

Vector mawarikomi(int power)
{
    int ir_deg = irDeg();
    int ir_dis = irDis();
    Vector vec;

    if (ir_dis < 600) // 近いなら
    {
        if (abs(ir_deg) < 60) // 前付近
        {
            float pos[4][2] = {{0, 0}, {10, 25}, {30, 40}, {60, 105}};
            float deg = lagrangeShifter(4, pos, abs(ir_deg));
            if (ir_deg < 0)
                deg *= -1;
            vec = Vector(deg, power);
        }
        else if (abs(ir_deg) < 100) // 右・左付近
        {
            int diff = (ir_deg > 0) ? 45 : -45; // 注意：posの最後の点に合わせること
            vec = Vector(ir_deg + diff, power);
        }
        else if (abs(ir_deg) < 140) // 右後ろ・左後ろ付近
        {
            int diff = (ir_deg > 0) ? 50 : -50;
            vec = Vector(ir_deg + diff, power);
        }
        else // 後ろ付近
        {
            int diff = (ir_deg > 0) ? 55 : -55;
            vec = Vector(ir_deg + diff, power);
        }
    }
    else // 遠いなら
    {
        vec = Vector(ir_deg, power);
    }

    return vec;
}

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

void attackWithGyro() // ジャイロで攻撃
{
    motorsPdProcess(&pd_gyro, bnoDeg(), 0); // ジャイロで姿勢制御

    if (catchSensor.read() == HIGH)
    {
        kicker1.kick(true);
    }
    else
    {
        kicker1.kick(false);
    }

    const int motor_line_max_power = 50;
    const int motor_ir_max_power = 90;

    if (line_timer.everReset() && line_timer.msTime() < 50UL)
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
        Vector vec = mawarikomi(motor_ir_max_power);
        motorsVectorMove(&vec);
    }
    else
    {
        motorsMove(fieldDeg(), 50);
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
        Vector vec = mawarikomi(motor_ir_max_power);
        motorsVectorMove(&vec);
    }
    else
    {
        motorsMove(fieldDeg(), 50);
    }
}