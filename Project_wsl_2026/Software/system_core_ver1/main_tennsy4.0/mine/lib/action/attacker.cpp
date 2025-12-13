#include "attacker.hpp"

static PD pd(0.8f, 0.0f); // ジャイロ用のPD調節値

void playAttacker(Attacker::Mode mode)
{
    if (mode == Attacker::Mode::YELLOWGOAL)
    {
        if (yellowGoalDetected())
        {
            motorsPdProcess(&pd, yellowGoalDeg(), 0); // カメラで姿勢制御
        }
        else
        {
            motorsPdProcess(&pd, bnoDeg(), 0); // ジャイロで姿勢制御
        }
    }
    else if (mode == Attacker::Mode::BLUEGOAL)
    {
        if (blueGoalDetected())
        {
            motorsPdProcess(&pd, blueGoalDeg(), 0); // カメラで姿勢制御
        }
        else
        {
            motorsPdProcess(&pd, bnoDeg(), 0); // ジャイロで姿勢制御
        }
    }
    else
    {
        motorsPdProcess(&pd, bnoDeg(), 0); // ジャイロで姿勢制御
    }

    bool is_motors_stop = false;
    Vector moveVec;
    if (lineRingDetected())
    {
        fullColorLed1.rgbLightUp(100, 0, 0);

        if (lineRingFirstDetedcted() || lineRingDetectingTime() < 80UL)
        {
            is_motors_stop = true;
        }
        else
        {
            if (abs(diffDeg(lineRingFirstDeg(), lineRingDeg())) < 100)
            {
                int power = (int)roundf((100.0f - lineRingDis()) * 0.40f + 40.0f);

                moveVec = Vector(lineRingDeg() + 180, power);
            }
            else
            {
                moveVec = Vector(lineRingDeg(), 100);
            }
        }
    }
    else if (irDetected())
    {
        if (irDeg() > -15 && irDeg() < 15)
        {
            // 50 = a * 15 + b
            // 95 = a * 0 + b
            // 50 - 95 = a * 15
            // -45 = a * 15
            // b = 95, a = -3
            int power = (-3 * abs(irDeg()) + 95);
            moveVec = Vector(0, power); // 前進
        }
        else if (abs(irDeg()) < 35)
        {
            // 70 = a * 35 + b
            // 50 = a * 15 + b
            // 20 = 20 * a
            // a = 1, b = 35
            int power = (abs(irDeg()) + 35);

            if (irDis() < 555)
            {
                int deg = irDeg() > 0 ? 90 : -90;

                moveVec = Vector(deg, power); // 赤外線センサーの方向へ移動
            }
            else
            {
                // int deg = int(roundf(float(irDeg() * irDeg()) * (irDeg() > 0 ? 0.071f : -0.071f)));
                int diff = int(irVal() * 0.100f);
                int deg = irDeg() > 0 ? irDeg() + diff : irDeg() - diff;

                moveVec = Vector(deg, power); // 赤外線センサーの方向へ移動
            }
        }
        else if (irDis() > 600.0f)
        {
            moveVec = Vector(irDeg(), 95);
        }
        else
        {
            if (abs(irDeg()) < 70)
            {
                int diff = int(irVal() * 0.088f);

                moveVec = Vector(irDeg() > 0 ? irDeg() + diff : irDeg() - diff, 70); // 赤外線センサーの方向へ移動
            }
            else
            {
                int diff = int(irVal() * 0.099f);

                moveVec = Vector(irDeg() > 0 ? irDeg() + diff : irDeg() - diff, 80); // 赤外線センサーの方向へ移動
            }
        }
    }

    if (is_motors_stop)
    {
        motorsStop(); // モータ停止
    }
    else if (moveVec.is_empty())
    {
        motorsPdMove(); // 回転のみ
    }
    else
    {
        motorsVectorMove(&moveVec); // 回り込み
    }
}