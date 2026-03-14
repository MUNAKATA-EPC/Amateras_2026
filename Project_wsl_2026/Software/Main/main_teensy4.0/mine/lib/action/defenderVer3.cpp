#include "defenderVer3.hpp"

// 攻撃ゴールの位置
static bool attack_goal_detected;
static int attack_goal_deg;
static float attack_goal_dis;
// 守備ゴールの位置
static bool defence_goal_detected;
static int defence_goal_deg;
static float defence_goal_dis;

// ディフエンダーのフォーム
enum DfForm
{
    ATTACKER, // ATTACKERフォーム
    DEFENCE,  // DEFENCEフォーム
    KIKAN     // KIKANフォーム
};

// ライン自己位置推定関数
enum LinePosition
{
    Yoko,
    Tate,
    Haji,
    None
};
LinePosition linePositionCheck()
{
    LineChunk line_chunk[16];
    int line_chunk_count = lineChunkCount(line_chunk);

    if (line_chunk_count == 1)
    {
        if (abs(line_chunk[0].deg) > 45 && abs(line_chunk[0].deg) < 135) // 縦線上にいる
        {
            return Tate;
        }
        else // 横線上にいる
        {
            return Yoko;
        }
    }
    else if (line_chunk_count == 2)
    {
        if (abs(lineRingDeg()) > 45 && abs(lineRingDeg()) < 135) // 縦線上にいる
        {
            return Tate;
        }
        else // 横線上にいる
        {
            return Yoko;
        }
    }
    else // if (line_chunk_count >= 3) // 端にいる
    {
        return Haji;
    }

    // if (line_chunk_count == 0)
    return None;
}

// ライントレース関数
void lineTraceMove(int move_deg, int move_power, int line_trace_power)
{
    Vector move_vector = Vector(nearSessenDeg(lineRingDeg(), move_deg), move_power);
    Vector line_trace_vector = Vector(lineRingDeg(), lineRingDis() * line_trace_power / 100.0f);
    Vector total_vector = move_vector + line_trace_vector;
    motorsVectorMove(&total_vector);
}

// ボールが正面にあったら止まるライントレース関数
void lineTraceMoveToDefence(int ir_deg, int move_deg, int move_power, int line_trace_power)
{
    if (abs(ir_deg) < 10)
    {
        lineTrace(0, 0, line_trace_power);
    }
    else if (abs(ir_deg) < 20)
    {
        int power = move_power * (abs(ir_deg) - 10) / 10;
        lineTrace(move_deg, power, line_trace_power);
    }
    else
    {
        lineTrace(move_deg, move_power, line_trace_power);
    }
}

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

    // フォーム決定
    DfForm df_form;

    bool is_my_defence_area =
        abs(fieldDeg()) < 90 &&
        defence_goal_detected &&
        abs(defence_goal_deg) > 90 &&
        defence_goal_dis < 85 &&
        ussLeftDis() > 30 &&
        ussRightDis() > 30;

    if (is_my_defence_area && lineRingDetected()) // DEFENCE動作
    {
        df_form = DfForm::DEFENCE;
    }
    else // KIKAN処理
    {
        df_form = DfForm::KIKAN;
    }

    // 動作決定　＆　制御
    motorsPdProcess(&pd_gyro, bnoDeg(), 0);

    if (df_form == DfForm::ATTACKER) // ATTACKERフォーム
    {
        playAttacker((mode == Defender::Mode::YELLOWGOAL) ? Attacker::Mode::YELLOWGOAL : Attacker::Mode::BLUEGOAL);
    }
    else if (df_form == DfForm::DEFENCE) // DEFENCEフォーム
    {
        if (irDetected())
        {
            int ir_defence_deg = (irDeg() - (defence_goal_deg + 180) + 360) % 360; // ゴールを基準としたボールの角度

            if (lineSideRightDetected() && lineSideLeftDetected())
            {
                // トレースせずに横移動
                if (irDeg() > 0) // ボールが右側にある
                {
                    lineTraceToDefence(irDeg(), 90, 80, 0);
                }
                else // ボールが左側にある
                {
                    lineTraceToDefence(irDeg(), -90, 80, 0);
                }
            }
            else
            {
                LinePosition line_position = linePositionCheck(); // Noneは考えられない

                if (line_position == LinePosition::Yoko)
                {
                    // 横トレース
                    int move_deg = (irDeg() > 0) ? 90 : -90;
                    lineTraceMoveToDefence(irDeg(), move_deg, 80, 15);
                }
                else if (line_position == LinePosition::Tate)
                {
                    if (abs(diffDeg(defence_goal_deg, irDeg())) < 90) // ロボット|ゴール|ボール　この状況の場合
                    {
                        // 前進トレース
                        lineTraceToDefence(irDeg(), 0, 80, 15);
                    }
                    else // ボール ロボット|ゴール|　この状況の場合
                    {
                        if (abs(irDeg()) < 90) // 前方にボールがある
                        {
                            // 前進トレース
                            lineTraceToDefence(irDeg(), 0, 80, 15);
                        }
                        else // 後方にボールがある
                        {
                            // 後退トレース（減速）
                            lineTraceMoveToDefence(irDeg(), 180, 40, 15);
                        }
                    }
                }
                else // if(line_position == LinePosition::Haji)
                {
                    // 端移動（前進）
                    lineTraceMoveToDefence(irDeg(), 0, 40, 15);
                }
            }
        }
        else
        {
            // 定位置移動
            LinePosition line_position = linePositionCheck(); // Noneは考えられない

            if (line_position == LinePosition::Yoko)
            {
                if (abs(defence_goal_deg) > 160)
                {
                    lineTraceMove(0, 0, 15);
                }
                else
                {
                    if (defence_goal_deg > 0) // コート右側にいる
                    {
                        lineTraceMove(90, 80, 15);
                    }
                    else // コート左側にいる
                    {
                        lineTraceMove(-90, 80, 15);
                    }
                }
            }
            else if (line_position == LinePosition::Tate)
            {
                lineTraceMove(0, 80, 15);
            }
            else // if(line_position == LinePosition::Haji)
            {
                lineTraceMove(0, 80, 15);
            }
        }
    }
    else if (df_form == DfForm::KIKAN) // KIKANフォーム
    {
        if (lineRingDetected())
        {
            // ラインから逃れる動き
            motorsMove(lineRingDeg() + 180, 80);
        }
        else if (defence_goal_detected)
        {
            // ゴールに向かう動き
            motorsMove(defence_goal_deg, 60);
        }
        else
        {
            if (abs(fieldDeg()) > 100) // コート前方にいる
            {
                // 守備ゴールに向かう動き
                motorsMove(fieldDeg(), 60);
            }
            else // コート後方にいる
            {
                if (fieldDeg() > 0) // コート右側にいる
                {
                    // 左下に移動
                    motorsMove(135, 60);
                }
                else
                {
                    // 右下に移動
                    motorsMove(-135, 60);
                }
            }
        }
    }
}