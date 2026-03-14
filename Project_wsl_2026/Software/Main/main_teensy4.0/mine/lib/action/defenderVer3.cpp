#include "defenderVer3.hpp"

// 攻撃ゴールの位置
static bool attack_goal_detected;
static int attack_goal_deg;
static float attack_goal_dis;
// 守備ゴールの位置
static bool defence_goal_detected;
static int defence_goal_deg;
static float defence_goal_dis;

// ディフエンダーの動作
enum Movement
{
    // ATTACKER動作
    ATTACKER,
    // DEFENCE動作
    DEFENCE_YOKO,
    DEFENCE_TATE,
    DEFENCE_HAJI,
    DEFENCE_RETURN,
    // KIKAN動作
    KIKAN_CLOSE,
    KIKAN_AWAY,
    // ESCAPE_LINE動作
    ESCAPE_LINE
};

//// ディフエンダーメイン ////
void playDefenderVer3(Defender::Mode mode)
{
    // 角度更新
    if (mode == Defender::Mode::YELLOWGOAL)
    {
        attack_goal_detected = yellowGoalDetected();
        attack_goal_deg = yellowGoalDeg();
        attack_goal_dis = yellowGoalDis();

        defence_goal_detected = blueGoalDetected();
        defence_goal_deg = blueGoalDeg();
        defence_goal_dis = blueGoalDis();
    }
    else // if (mode == Defender::Mode::BLUEGOAL)
    {
        defence_goal_detected = yellowGoalDetected();
        defence_goal_deg = yellowGoalDeg();
        defence_goal_dis = yellowGoalDis();

        attack_goal_detected = blueGoalDetected();
        attack_goal_deg = blueGoalDeg();
        attack_goal_dis = blueGoalDis();
    }

    // 動作決定
    Movement movement;

    bool is_my_defence_area =
        abs(fieldDeg()) < 90 &&
        defence_goal_detected &&
        abs(defence_goal_deg) > 190 &&
        defence_goal_dis < 85 &&
        ussLeftDis() > 30 &&
        ussRightDis() > 30;

    if (is_my_defence_area && lineRingDetected()) // DEFENCE動作
    {
        if (irDetected())
        {
            LineChunk line_chunk[16];
            int line_chunk_count = lineChunkCount(line_chunk);

            if (lineSideRightDetected() && lineSideLeftDetected())
            {
                movement = DEFENCE_YOKO;
            }
            else if (line_chunk_count == 1)
            {
                if (abs(line_chunk[0].deg) > 45 && abs(line_chunk[0].deg) < 135) // 縦線上にいる
                {
                    movement = DEFENCE_TATE;
                }
                else // 横線上にいる
                {
                    movement = DEFENCE_YOKO;
                }
            }
            else if (line_chunk_count == 2)
            {
                if (abs(lineRingDeg()) > 45 && abs(lineRingDeg()) < 135) // 縦線上にいる
                {
                    movement = DEFENCE_TATE;
                }
                else // 横線上にいる
                {
                    movement = DEFENCE_YOKO;
                }
            }
            else // if (line_chunk_count >= 3)
            {
                movement = DEFENCE_HAJI;
            }
        }
        else
        {
            movement = DEFENCE_RETURN;
        }
    }
    else // KIKAN処理
    {
        if (lineRingDetected())
        {
            movement = ESCAPE_LINE;
        }
        else
        {
            if (abs(fieldDeg() > 100)) // コート前方にいる
            {
                movement = KIKAN_CLOSE;
            }
            else // コート後方にいる
            {
                if (is_my_defence_area)
                {
                    movement = KIKAN_AWAY;
                }
                else
                {
                    movement = KIKAN_CLOSE;
                }
            }
        }
    }

    // 制御
    motorsPdProcess(&pd_gyro, bnoDeg(), 0);

    switch (movement)
    {
    case ATTACKER:
    {
        playAttacker((mode == Defender::Mode::YELLOWGOAL) ? Attacker::Mode::YELLOWGOAL : Attacker::Mode::BLUEGOAL);
        break;
    }

    case DEFENCE_YOKO:
    {
        break;
    }

    case DEFENCE_TATE:
    {
        break;
    }

    case DEFENCE_HAJI:
    {
        break;
    }

    case DEFENCE_RETURN:
    {
        break;
    }

    case KIKAN_CLOSE:
    {
        break;
    }

    case KIKAN_AWAY:
    {
        break;
    }

    case ESCAPE_LINE:
    {
        break;
    }
    }
}