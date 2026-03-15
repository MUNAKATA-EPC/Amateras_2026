#include "attacker.hpp"

Vector mawarikomi(int max_power)
{
    int ir_deg = irDeg();
    int ir_dis = irDis();
    bool ir_detected = irDetected();

    if (ir_detected) // ボールがある
    {
        if (catchSensor.read() == HIGH) // ボールを持っているときは直進する
        {
            return Vector(0, max_power);
        }
        if (ir_dis <= 300) // 近い
        {
            // 角度
            float deg = 0xFF, power = max_power;
            if (abs(ir_deg) <= 10) // 前
            {
                deg = 0;
                power *= 1.0f;
            }
            else if (abs(ir_deg) <= 45) // 前付近
            {
                float k = (ir_deg > 0) ? 1.0f : -1.0f;
                deg = ir_deg + k * 50.0f;

                power *= map(ir_deg, 10, 45, 85, 100) / 100.0f; // 少し減速する
            }
            else if (abs(ir_deg) <= 100) // 中央付近
            {
                float k = (ir_deg > 0) ? 1.0f : -1.0f;
                deg = ir_deg + k * 50.0f;

                power *= 1.0f;
            }
            else
            {
                float k = (ir_deg > 0) ? 1.0f : -1.0f;
                deg = ir_deg + k * 50.0f;

                power *= 1.0f;
            }

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
    const int motor_ir_max_power = 95;

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
    /*else if (ussRightDetected() && ussLeftDetected() && (ussRightDis() <= 30 || ussLeftDis() <= 30))
    {
        if (abs(irDeg()) < 90)
        {
            motorsMove(0, motor_ir_max_power * 0.8);
        }
        else
        {
            motorsMove(180, motor_ir_max_power * 0.8);
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
    const int motor_ir_max_power = 95;

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
    else if (lineRingDetected() || lineSideRightDetected() || lineSideLeftDetected()) // エンジェルライン
    {
        motorsMove(fieldDeg(), motor_line_max_power);
        line_timer.reset();
    }
    /*else if (ussRightDetected() && ussLeftDetected() && (ussRightDis() <= 30 || ussLeftDis() <= 30) && abs(bnoDeg()) < 15)
    {
        if (abs(irDeg()) < 90)
        {
            motorsMove(0, motor_ir_max_power * 0.8);
        }
        else
        {
            motorsMove(180, motor_ir_max_power * 0.8);
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