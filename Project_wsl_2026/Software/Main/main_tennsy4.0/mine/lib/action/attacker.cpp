#include "attacker.hpp"

Vector mawarikomi(int max_power)
{
    int ir_deg = irDeg();
    int ir_dis = irDis();
    bool ir_detected = irDetected();

    if (ir_detected) // ボールがある
    {
        if (ir_dis < 500) // 近い
        {
            // 角度
            float deg = 0xFF;
            if (irDeg() >= -22 && irDeg() <= 22) // -22°～22°
            {
                float pos[4][2] = {{0, 0}, {8, 8}, {15, 25}, {22, 40}};
                deg = ((ir_deg > 0) ? 1 : -1) * lagrangeShifter(4, pos, abs(ir_deg));
            }
            else if (irDeg() >= -45 && irDeg() <= 45) //-45°～45°
            {
                float pos[4][2] = {{22, 40}, {30, 75}, {35, 85}, {45, 100}};
                deg = ((ir_deg > 0) ? 1 : -1) * lagrangeShifter(4, pos, abs(ir_deg));
            }
            else if (irDeg() >= -90 && irDeg() <= 90) // -90°～90°
            {
                float pos[4][2] = {{45, 100}, {50, 140}, {60, 155}, {90, 180}};
                deg = ((ir_deg > 0) ? 1 : -1) * lagrangeShifter(4, pos, abs(ir_deg));
            }
            else if (irDeg() >= -90 && irDeg() <= 90) // -135°～135°
            {
                float pos[4][2] = {{90, 180}, {110, 190}, {125, 200}, {135, 210}};
                deg = ((ir_deg > 0) ? 1 : -1) * lagrangeShifter(4, pos, abs(ir_deg));
            }
            else // if (irDeg() >= -180 && irDeg() <= 180) // -180°～180°
            {
                float pos[4][2] = {{135, 210}, {140, 220}, {155, 240}, {180, 270}};
                deg = ((ir_deg > 0) ? 1 : -1) * lagrangeShifter(4, pos, abs(ir_deg));
            }

            // パワー
            float power = max_power;

            return Vector(deg, power);
        }
        else // 遠い
        {
            return Vector(ir_deg, max_power);
        }
    }

    return Vector(0, 0.0f);
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