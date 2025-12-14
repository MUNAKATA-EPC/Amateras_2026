#include "attacker.hpp"

void attackWithGyro();
void attackWithCam(bool attack_goal_detacted, int attack_goal_deg, int attack_goal_dis, bool defence_goal_detected, int defence_goal_deg, int defence_goal_dis);

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

Timer line_timer;

void attackWithGyro() // ジャイロで攻撃
{
    motorsPdProcess(&pd_gyro, bnoDeg(), 0); // ジャイロで姿勢制御

    const int motor_line_max_power = 90;
    const int motor_ir_max_power = 90;

    if (line_timer.everReset() && line_timer.msTime() < 500UL)
    {
        if (lineRingDetected())
        {
            if (lineRingDetectingTime() < 250UL) // ライン反応後250ms間はモータは停止させる
            {
                motorsStop();
            }
            else if (abs(diffDeg(lineRingDeg(), lineRingFirstDeg())) > 90) // ハーフアウトした
            {
                motorsMove(lineRingDeg(), motor_line_max_power);
            }
            else
            {
                motorsMove(lineRingDeg() + 180, motor_line_max_power);
            }
        }
        else
        {
            motorsMove(lineRingFirstDeg() + 180, motor_line_max_power);
        }
    }
    else if (lineRingDetected())
    {
        if (lineRingDetectingTime() < 250UL) // ライン反応後250ms間はモータは停止させる
        {
            motorsStop();
        }
        else if (abs(diffDeg(lineRingDeg(), lineRingFirstDeg())) > 90) // ハーフアウトした
        {
            motorsMove(lineRingDeg(), motor_line_max_power);
        }
        else
        {
            motorsMove(lineRingDeg() + 180, motor_line_max_power);
        }

        line_timer.reset();
    }
    else if (irDetected())
    {
        if (irDeg() >= -6 && irDeg() <= 6)
        {
            motorsMove(0, motor_ir_max_power);
        }
        else if (irDeg() >= -10 && irDeg() <= 10)
        {
            motorsMove(irDeg() * 1.25f, motor_ir_max_power);
        }
        else
        {
            if (irDis() >= 56.0f)
            {
                motorsMove(irDeg(), motor_ir_max_power);
            }
            else
            {
                int diff = irVal();

                if (irDeg() > 0) // 左にいるとき
                {
                    if (irDeg() <= 60)
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
                    if (irDeg() >= -60)
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

Timer cam_pd_timer;

void attackWithCam(bool attack_goal_detacted, int attack_goal_deg, int attack_goal_dis, bool defence_goal_detected, int defence_goal_deg, int defence_goal_dis) // カメラで攻撃
{
    if (attack_goal_detacted && cam_pd_timer.everReset() && cam_pd_timer.msTime() < 600UL)
    {
        motorsPdProcess(&pd_cam, attack_goal_deg, 0); // ジャイロで姿勢制御
    }
    else if (attack_goal_detacted && attack_goal_dis < 120)
    {
        motorsPdProcess(&pd_cam, attack_goal_deg, 0); // ジャイロで姿勢制御
        cam_pd_timer.reset();
    }
    else
    {
        motorsPdProcess(&pd_gyro, bnoDeg(), 0); // ジャイロで姿勢制御
    }

    const int motor_line_max_power = 90;
    const int motor_ir_max_power = 90;

    if (lineRingDetected())
    {
        if (lineRingDetectingTime() < 80UL) // ライン反応後80ms間はモータは停止させる
        {
            motorsStop();
        }
        else if (attack_goal_detacted && attack_goal_dis < 80)
        {
            motorsMove(attack_goal_deg + 180, motor_line_max_power);
        }
        else if (defence_goal_detected && defence_goal_dis < 80)
        {
            motorsMove(defence_goal_deg + 180, motor_line_max_power);
        }
        else
        {
            if (abs(diffDeg(lineRingDeg(), lineRingFirstDeg())) > 90) // ハーフアウトした
            {
                motorsMove(lineRingDeg(), motor_line_max_power);
            }
            else
            {
                motorsMove(lineRingDeg() + 180, motor_line_max_power);
            }
        }
    }
    else if (irDetected())
    {
        if (irDeg() > -10 && irDeg() > 10)
        {
            motorsMove(0, motor_ir_max_power);
        }
        else
        {
            if (irDis() > 560)
            {
                motorsMove(irDeg(), motor_ir_max_power);
            }
            else
            {
                if (irDeg() > 0) // 左にいるとき
                {
                    if (irDeg() <= 60)
                    {
                        motorsMove(irDeg() + 80, motor_ir_max_power);
                    }
                    else if (irDeg() <= 160)
                    {
                        motorsMove(irDeg() + 55, motor_ir_max_power);
                    }
                    else
                    {
                        motorsMove(irDeg() + 80, motor_ir_max_power);
                    }
                }
                else // 右にいるとき
                {
                    if (irDeg() >= -60)
                    {
                        motorsMove(irDeg() - 80, motor_ir_max_power);
                    }
                    else if (irDeg() >= -160)
                    {
                        motorsMove(irDeg() - 55, motor_ir_max_power);
                    }
                    else
                    {
                        motorsMove(irDeg() - 80, motor_ir_max_power);
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