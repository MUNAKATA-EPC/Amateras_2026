#include "attacker.hpp"

float smoothFunc(float x1, float x2, float y1, float y2, float input_x)
{
    float x = fabsf(input_x);
    if (0 <= x && x <= x1)
    {
        return y1;
    }
    else if (x1 < x && x < x2)
    {

        float p = x2 - x1;
        float q = y2 - y1;

        return pow(x - x1, 3) * (-2 * q / (p * p * p)) + pow(x - x1, 2) * (3 * q / (p * p)) + y1; // 3次関数
        // return pow(x - x1, 4) * (-q / (p * p * p * p)) + pow(x - x1, 2) * (2 * q / (p * p)) + y1; // 4次関数
    }
    else
    {
        return y2;
    }
}

Vector mawarikomi()
{
    if (!irDetected())
        return Vector(0, 0.0f);

    float ir_rad = (float)radians(irDeg());
    float ir_dis = irDis();
    float ir_x = irX();
    float ir_y = irY();

    Vector vec;

    if (ir_dis > 580)
    {
        vec = Vector(degrees(ir_rad), 95.0f);
    }
    else if (ir_x > 0.0f && fabsf(ir_y) < 150.0f)
    {
        if (catchSensor.read() == HIGH)
        {
            vec = Vector(0.0f, 95.0f);
        }
        else
        {
            // rad計算
            float dis_x = smoothFunc(35.0f, 100.0f, 60.0f, 300.0f, ir_y);
            // length計算
            float length_temp;
            if (fabsf(ir_y) < 35.0f)
            {
                length_temp = 95.0f;
            }
            else
            {
                length_temp = smoothFunc(35.0f, 140.0f, 60.0f, 95.0f, ir_y);
            }

            Vector vec_temp = Vector(dis_x, 0, ir_x, ir_y);
            vec_temp = vec_temp * length_temp / vec_temp.length();

            vec = vec_temp;
        }
    }
    else
    {
        const float r = 260.0f;
        const float pi = (float)PI;

        float rad_temp;
        float length_temp = 95.0f;

        if (r <= ir_dis)
        {
            float alpha = asinf(r / ir_dis);

            if (ir_rad >= 0)
            {
                rad_temp = ir_rad + alpha;
            }
            else // (ir_rad < 0)
            {
                rad_temp = ir_rad - alpha;
            }
        }
        else // (r > ir_dis)
        {
            if (ir_rad >= 0)
            {
                rad_temp = ir_rad + pi / 2;
            }
            else // (ir_rad < 0)
            {
                rad_temp = ir_rad - pi / 2;
            }
        }

        vec = Vector(degrees(rad_temp), length_temp);
    }

    static float smooth_length = 60.0f;

    if (vec.length() >= smooth_length)
    {
        smooth_length += 1.5f;
    }
    else
    {
        smooth_length -= 1.5f;
    }

    smooth_length = constrain(smooth_length, 0.0f, 100.0f);

    Vector final_vec = Vector(vec.deg(), smooth_length);
    return final_vec;
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
    else if (irDetected())
    {
        Vector vec = mawarikomi();
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
    if (attack_goal_detected && catchSensor.read() == HIGH)
    {
        motorsPdProcess(&pd_cam, attack_goal_deg, 0); // カメラで姿勢制御
    }
    else
    {
        motorsPdProcess(&pd_gyro, bnoDeg(), 0); // ジャイロで姿勢制御
    }

    if (catchSensor.read() == HIGH && attack_goal_detected && fabsf(attack_goal_deg) <= 70)
    {
        kicker1.kick(true);
    }
    else
    {
        kicker1.kick(false);
    }

    const int motor_line_max_power = 50;

    if (attack_goal_detected && attack_goal_dis < 60 && fabsf(fieldDeg()) > 90)
    {
        motorsMove(attack_goal_deg + 180, motor_line_max_power);
    }
    else if (defence_goal_detected && defence_goal_dis < 60 && fabsf(fieldDeg()) < 90)
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
    else if (irDetected())
    {
        Vector vec = mawarikomi();
        motorsVectorMove(&vec);
    }
    else
    {
        motorsPdMove();
    }
}