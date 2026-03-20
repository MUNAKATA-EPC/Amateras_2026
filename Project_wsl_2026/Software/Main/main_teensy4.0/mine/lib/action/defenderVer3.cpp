#include "defenderVer3.hpp"

// 攻撃ゴールの位置
static bool attack_goal_detected;
static float attack_goal_deg;
static float attack_goal_dis;
// 守備ゴールの位置
static bool defence_goal_detected;
static float defence_goal_deg;
static float defence_goal_dis;

// ディフエンダーのフォーム
enum DfForm
{
    ATTACKER,           // ATTACKERフォーム
    DEFENCE,            // DEFENCEフォーム
    DEFENCE_LINE_HOSEI, // DEFENCE_LINE_HOSEIフォーム（自分陣地でラインから離れた場合一時的にラインに戻ろうとする）
    KIKAN               // KIKANフォーム
};

// ラインの絶対角を出す関数
float lineAbsoluteRingDeg()
{
    if (!lineRingDetected())
        return 0xFF;

    return normalizeDeg(lineRingDeg() - bnoDeg());
}

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
        float absolute_line_chunk_deg = normalizeDeg(line_chunk[0].deg - bnoDeg());
        if (fabsf(absolute_line_chunk_deg) > 45 && fabsf(absolute_line_chunk_deg) < 135) // 縦線上にいる
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
        if (fabsf(lineAbsoluteRingDeg()) > 45 && fabsf(lineAbsoluteRingDeg()) < 135) // 縦線上にいる
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
void lineTraceMove(float move_deg, float move_power, float line_trace_power)
{
    Vector move_vector = Vector(nearSessenDeg(lineRingDeg(), move_deg), move_power);
    Vector line_trace_vector = Vector(lineRingDeg(), lineRingDis() * line_trace_power / 100.0f);
    Vector total_vector = move_vector + line_trace_vector;
    motorsVectorMove(&total_vector);
}

//// ディフエンダーメイン ////
void playDefenderVer3(Defender::Mode mode)
{
    //// 角度更新
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

    //// フォーム決定
    DfForm df_form;
    LinePosition line_position = linePositionCheck();

    // 守備エリア条件
    bool my_defence_area =
        fabsf(fieldDeg()) < 90 &&
        defence_goal_detected &&
        fabsf(defence_goal_deg) > 90 &&
        defence_goal_dis < 85;

    // 攻撃フォーム切り替え条件
    static bool my_defence_point = false; // 守備フォーム かつ ボールとゴールの線分上にいる時true
    static bool old_my_defence_point = false;

    static Timer my_defence_point_timer; // 守備フォーム かつ ボールとゴールの線分上にいる時間計測用
    if (old_my_defence_point == false && my_defence_point == true)
    {
        my_defence_point_timer.reset();
    }

    old_my_defence_point = my_defence_point; // 記録
    my_defence_point = false;                // リセット

    // 攻撃フォーム取り消し条件
    static bool attack_form = false; // 現在ATTACKERフォームかどうか
    static bool old_attack_form = false;
    static Timer attacker_form_timer; // ATTAKCERフォーム継続時間

    if (attack_form == true && old_attack_form == false)
    {
        attacker_form_timer.reset();
    }

    old_attack_form = attack_form; // 記録

    if (attack_form)
    {
        if (lineRingDetected() && attacker_form_timer.msTime() > 1500UL) // ラインに反応したらATTACKERフォーム取り消し
        {
            attack_form = false;
        }
        else if (attacker_form_timer.msTime() > 5000UL)
        {
            attack_form = false;
        }
    }

    // 代入
    if (attack_form)
    {
        df_form = DfForm::ATTACKER; // ATTACKERフォーム
    }
    else if (my_defence_area)
    {
        if (lineRingDetected())
        {
            if (old_my_defence_point && my_defence_point_timer.msTime() > 2500UL)
            // 前回守備フォーム かつ ボールとゴールの線分上にいる かつ その状況が2.5s以上続いている
            {
                df_form = DfForm::ATTACKER; // ATTACKERフォーム
                attack_form = true;         // アタックフォームに切り替え
            }
            else
            {
                df_form = DfForm::DEFENCE; // DEFENCEフォーム
            }
        }
        else if (lineRingLastDetectingTime() < 5000UL)
        {
            df_form = DfForm::DEFENCE_LINE_HOSEI; // DEFENCE_LINE_HOSEIフォーム
        }
        else
        {
            df_form = DfForm::KIKAN; // KIKANフォーム
        }
    }
    else // KIKAN処理
    {
        df_form = DfForm::KIKAN; // KIKANフォーム
    }

    //// 守備制御
    motorsPdProcess(&pd_gyro, bnoDeg(), 0);

    if (df_form == DfForm::ATTACKER) // ATTACKERフォーム
    {
        playAttacker((mode == Defender::Mode::YELLOWGOAL) ? Attacker::Mode::YELLOWGOAL : Attacker::Mode::BLUEGOAL);
    }
    else if (df_form == DfForm::DEFENCE) // DEFENCEフォーム
    {
        const int line_trace_power = 10; // ライントレースに使う出力

        if (irDetected())
        {
            float ir_defence_deg = nearSessenDeg(defence_goal_deg, irDeg()); // ゴールの接線方向の角度の内ボールの角に近いほう

            // 出力減少処理
            float diff_from_ball_to_goal = fabsf(diffDeg(defence_goal_deg, irDeg()));
            float abs_defence_y = sinf(radians(diff_from_ball_to_goal)) * irDis(); // ゴールの方向を基準とした見たボールのy成分

            float power_down_gain = 1.0f; // 減速ゲイン

            if (abs_defence_y < 180.0f) // (ゴール) - (ロボット) - (ボール) または (ゴール) - (ボール) - (ロボット)
            {
                if (diff_from_ball_to_goal > 90) // (ゴール) - (ロボット) - (ボール)
                {
                    my_defence_point = true;

                    if (abs_defence_y < 80.0f)
                    {
                        power_down_gain = 0.0f;
                    }
                    else
                    {
                        power_down_gain = map(abs_defence_y, 80, 180, 0.0f, 100.0f) / 100.0f;
                    }
                }
                else // (ゴール) - (ボール) - (ロボット)
                {
                    power_down_gain = 0.6f; // 減速
                }
            }

            // 守備
            if (line_position == LinePosition::Yoko)
            {
                if (lineSideRightDetected() && lineSideLeftDetected())
                {
                    // トレースせずに横移動
                    if (irDeg() > 0) // ボールが左側にいる
                    {
                        lineTraceMove(90, 95 * power_down_gain, 0);
                    }
                    else // ボールが右側にいる
                    {
                        lineTraceMove(-90, 95 * power_down_gain, 0);
                    }
                }
                else
                {
                    // 横トレース
                    lineTraceMove(ir_defence_deg, 80 * power_down_gain, line_trace_power);
                }
            }
            else if (line_position == LinePosition::Tate)
            {
                if (defence_goal_deg > 0 && irDeg() > 0) // |_(ボール)_(ゴール)_(ロボット)_| この状況である可能性が高い
                {
                    lineTraceMove(0, 60, line_trace_power);
                }
                else if (defence_goal_deg < 0 && irDeg() < 0) // |_(ロボット)_(ゴール)_(ボール)_| この状況である可能性が高い
                {
                    lineTraceMove(0, 60, line_trace_power);
                }
                else
                {
                    if (fabsf(irDeg()) < 90) // 前にボールがある
                    {
                        lineTraceMove(ir_defence_deg, 60 * power_down_gain, line_trace_power);
                    }
                    else // 後ろにボールがある
                    {
                        lineTraceMove(ir_defence_deg, 30 * power_down_gain, line_trace_power);
                    }
                }
            }
            else // if(line_position == LinePosition::Haji) // Noneは考えられない
            {
                // 端移動（前進）
                lineTraceMove(0, 30 * power_down_gain, line_trace_power);
            }
        }
        else
        {
            // 定位置移動
            if (line_position == LinePosition::Yoko)
            {
                if (fabsf(defence_goal_deg) > 170)
                {
                    lineTraceMove(0, 0, line_trace_power);
                }
                else
                {
                    if (defence_goal_deg > 0) // コート右側にいる
                    {
                        lineTraceMove(90, 80, line_trace_power);
                    }
                    else // コート左側にいる
                    {
                        lineTraceMove(-90, 80, line_trace_power);
                    }
                }
            }
            else if (line_position == LinePosition::Tate)
            {
                lineTraceMove(0, 80, line_trace_power);
            }
            else // if(line_position == LinePosition::Haji) // Noneは考えられない
            {
                lineTraceMove(0, 80, line_trace_power);
            }
        }
    }
    else if (df_form == DfForm::DEFENCE_LINE_HOSEI) // DEFENCE_LINE_HOSEIフォーム
    {
        motorsMove(lineRingLastDeg(), 80);
    }
    else if (df_form == DfForm::KIKAN) // KIKANフォーム
    {
        if (lineRingDetected())
        {
            // ラインから逃れる動き
            if (fabsf(lineAbsoluteRingDeg()) > 135 &&
                fabsf(fieldDeg()) < 90) // |__(ロボット)__(ゴール)___________| この状況である可能性が高い
            {
                if (fieldDeg() > 0) // コート上で右にいる
                {
                    lineTraceMove(90, 30, 10); // ライントレースする
                }
                else
                {
                    lineTraceMove(-90, 30, 10); // ライントレースする
                }
            }
            else
            {
                motorsMove(fieldDeg(), 80);
            }
        }
        else if (defence_goal_detected)
        {
            // ゴールに向かう動き
            motorsMove(defence_goal_deg, 60);
        }
        else
        {
            motorsMove(180, 60); // 後ろに移動する
        }
    }
}