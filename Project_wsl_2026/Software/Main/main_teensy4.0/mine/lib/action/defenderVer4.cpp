#include "defenderVer4.hpp"

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
Vector lineTrace(float move_deg, float move_power, float line_trace_power)
{
    if (!lineRingDetected())
        return Vector(0, 0);

    move_deg = normalizeDeg(move_deg); // 正規化

    Vector move_vec = Vector(nearSessenDeg(lineRingDeg(), move_deg), move_power);
    Vector line_trace_vec = Vector(lineRingDeg(), lineRingDis() * line_trace_power / 100.0f);
    Vector vec = move_vec + line_trace_vec;
    return vec;
}

// ディフエンダー用ラントレース関数
Vector dfLineTrace(LinePosition posi, float move_deg, float move_power, float line_trace_power)
{
    if (!lineRingDetected())
        return Vector(0, 0);

    move_deg = normalizeDeg(move_deg); // 正規化

    float df_move_deg, df_move_power;
    if (posi == Haji)
    {
        df_move_deg = 0;
        df_move_power = 30;
    }
    else if (posi == Tate)
    {
        if (defence_goal_deg > 0) // コート右側
        {
            if (move_deg < -90 || move_deg > defence_goal_deg)
            {
                df_move_deg = 180;
                df_move_power = move_power;
            }
            else
            {
                df_move_deg = 0;
                df_move_power = move_power;
            }
        }
        else // コート左側
        {
            if (move_deg > 90 || move_deg < defence_goal_deg)
            {
                df_move_deg = 180;
                df_move_power = move_power;
            }
            else
            {
                df_move_deg = 0;
                df_move_power = move_power;
            }
        }
    }
    else if (posi == Yoko)
    {
        if (move_deg > 0)
        {
            df_move_deg = 90;
            df_move_power = move_power;
        }
        else
        {
            df_move_deg = -90;
            df_move_power = move_power;
        }
    }
    else // if (posi == None)
    {
        df_move_deg = 0;
        df_move_power = 0;
    }

    Vector vec;
    vec = lineTrace(df_move_deg, df_move_power, line_trace_power);

    return vec;
}

//// ディフエンダーメイン ////
void playDefenderVer4(Defender::Mode mode)
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
        if (irDetected())
        {
            float defence_ir_deg = nearSessenDeg(defence_goal_deg, irDeg()); // 守備ゴールの±90度の方向のうち、ボールの方向に近い角度

            float defence_ir_y = irDis() * sinf(radians(diffDeg(defence_goal_deg + 180, irDeg()))); // y方向の偏差
            float defence_ir_x = irDis() * cosf(radians(diffDeg(defence_goal_deg + 180, irDeg()))); // x方向の偏差

            Vector vec;
            if (defence_ir_x > 0 && fabsf(defence_ir_y) < 30.0f)
            {
                vec = lineTrace(0, 0, 50);
            }
            else
            {
                vec = dfLineTrace(line_position, defence_ir_deg, 90, 10);
            }
            motorsVectorMove(&vec);
        }
        else
        {
            // 定位置移動
            Vector vec;
            if (fabsf(defence_goal_deg) > 165)
            {
                vec = lineTrace(0, 0, 50);
            }
            else
            {
                float teiiti_deg = (defence_goal_deg > 0) ? defence_goal_deg - 90 : defence_goal_deg + 90;
                vec = dfLineTrace(line_position, teiiti_deg, 90, 10);
            }
            motorsVectorMove(&vec);
        }
    }
    else if (df_form == DfForm::DEFENCE_LINE_HOSEI) // DEFENCE_LINE_HOSEIフォーム
    {
        if (defence_goal_dis < 60)
        {
            motorsMove(defence_goal_deg + 180, 80);
        }
        else
        {
            motorsMove(lineRingLastDeg(), 80);
        }
    }
    else if (df_form == DfForm::KIKAN) // KIKANフォーム
    {
        if (lineRingDetected())
        {
            // ラインから逃れる動き
            if (fabsf(lineAbsoluteRingDeg()) > 135 &&
                fabsf(fieldDeg()) < 90) // |__(ロボット)__(ゴール)___________| この状況である可能性が高い
            {
                Vector vec;
                if (fieldDeg() > 0) // コート上で右にいる
                {
                    vec = lineTrace(90, 30, 10); // ライントレースする
                }
                else
                {
                    vec = lineTrace(-90, 30, 10); // ライントレースする
                }
                motorsVectorMove(&vec);
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