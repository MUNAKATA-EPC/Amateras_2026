#include "attacker.hpp"

static PD pd(0.8f, -10.0f); // ジャイロ用のPD調節値

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
        if (lineRingFirstDetedcted() || lineRingDetectingTime() < 50UL)
        {
            is_motors_stop = true;
        }
        else
        {
            if (abs(diffDeg(lineRingFirstDeg(), lineRingDeg())) < 80)
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
        if (abs(irDeg()) < 10)
        {
            moveVec = Vector(0, 80); // 前進
        }
        else if (abs(irDeg()) < 35)
        {
            int deg = int(roundf(float(irDeg() * irDeg()) * (irDeg() > 0 ? 0.0666f : -0.0666f)));

            moveVec = Vector(deg, 60); // 赤外線センサーの方向へ移動
        }
        else if (irDis() > 530.0f)
        {
            moveVec = Vector(irDeg(), 80);
        }
        else
        {
            if (abs(irDeg()) < 100)
            {
                int diff = int(irVal() * 0.066f);

                moveVec = Vector(irDeg() > 0 ? irDeg() + diff : irDeg() - diff, 80); // 赤外線センサーの方向へ移動
            }
            else
            {
                int diff = int(irVal() * 0.1f);

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