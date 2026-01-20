#include "attacker.hpp"

Vector mawarikomi(int max_power)
{
    int ir_deg = irDeg();
    int ir_dis = irDis();
    bool ir_detected = irDetected();

    if (ir_detected) // ボールがある
    {
        if (ir_dis <= 540) // 近い
        {
            // 角度
            float deg = 0xFF;
            if (irDeg() >= -45 && irDeg() <= 45) // -45°～45°
            {
                float pos[5][2] = {{0, 0}, {8, 10}, {20, 60}, {35, 95}, {45, 105}};
                deg = ((ir_deg > 0) ? 1 : -1.1f) * lagrangeShifter(5, pos, abs(ir_deg));
            }
            /*
            else if (irDeg() >= -45 && irDeg() <= 45) //-45°～45°
            {
                float pos[4][2] = {{22, 60}, {30, 75}, {35, 90}, {45, 100}};
                deg = ((ir_deg > 0) ? 1 : -1) * lagrangeShifter(4, pos, abs(ir_deg));
            }*/
            else
            {
                float k = (ir_deg > 0) ? 1.0f : -1.0f;
                deg = ir_deg + k * 60;
            }

            // deg = ir_deg + (deg - ir_deg) * map(ir_dis, 200, 500, 1.4f, 1.0f);

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

    static int old_line_ring_deg = 0;

    if (line_timer.everReset() && line_timer.msTime() < 50UL)
    {
        motorsMove(old_line_ring_deg + 180, motor_line_max_power);
    }
    else if (lineRingDetected()) // エンジェルライン
    {
        motorsMove(lineRingDeg() + 180, motor_line_max_power);
        old_line_ring_deg = lineRingDeg();
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

    if (attack_goal_detected && attack_goal_dis < 60)
    {
        motorsMove(attack_goal_deg + 180, motor_line_max_power);
    }
    else if (defence_goal_detected && defence_goal_dis < 60)
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
    /*else if (ussRightDetected() && ussLeftDetected() && ussRightDis() <= 36 && (ussRightDis() + ussLeftDis()) > 80)
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
    else if (ussRightDetected() && ussLeftDetected() && ussLeftDis() <= 36 && (ussRightDis() + ussLeftDis()) > 80)
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
        motorsPdMove();
    }
}