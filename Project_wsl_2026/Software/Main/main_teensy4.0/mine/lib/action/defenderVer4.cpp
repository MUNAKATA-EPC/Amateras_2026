#include "defenderVer4.hpp"

// ディフエンダーのフォーム
enum DfForm
{
    ATTACKER_DIRECT_LINETRACE, // ボールにダイレクトにライントレースで近づくフォーム
    ATTACKER,                  // playAttacker関数による攻撃フォーム
    DEFENCE,                   // 通常の守備フォーム
    DEFENCE_LINE_HOSEI,        // 自分陣地でラインから離れた場合一時的にラインに戻ろうとするフォーム
    KIKAN                      // 自分陣地に帰還するフォーム
};

// ディフエンダー用ラントレース関数
Vector dfLineTrace(LinePosition posi, float move_deg, float move_power, float line_trace_power)
{
    if (!lineRingDetected())
        return Vector(0, 0);

    move_deg = normalizeDeg(move_deg); // 正規化

    float df_move_deg, df_move_power, df_line_trace_power;
    if (posi == Haji)
    {
        if (defenceGoalDeg() > 0) // コート右側
        {
            if (move_deg < -90 || move_deg > 135)
            {
                df_move_deg = 0;
                df_move_power = 0;

                df_line_trace_power = 50;
            }
            else
            {
                df_move_deg = 0;
                df_move_power = move_power * 0.6f;

                df_line_trace_power = line_trace_power;
            }
        }
        else // コート左側
        {
            if (move_deg > 90 || move_deg < -135)
            {
                df_move_deg = 0;
                df_move_power = 0;

                df_line_trace_power = 50;
            }
            else
            {
                df_move_deg = 0;
                df_move_power = move_power * 0.6f;

                df_line_trace_power = line_trace_power;
            }
        }
    }
    else if (posi == Tate)
    {
        if (defenceGoalDeg() > 0) // コート右側
        {
            if (move_deg < -90 || move_deg > 135)
            {
                df_move_deg = 180;
                df_move_power = move_power * 0.6f;
            }
            else
            {
                df_move_deg = 0;
                df_move_power = move_power * 0.6f;
            }

            df_line_trace_power = line_trace_power;
        }
        else // コート左側
        {
            if (move_deg > 90 || move_deg < -135)
            {
                df_move_deg = 180;
                df_move_power = move_power * 0.6f;
            }
            else
            {
                df_move_deg = 0;
                df_move_power = move_power * 0.6f;
            }

            df_line_trace_power = line_trace_power;
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

        df_line_trace_power = line_trace_power;
    }
    else // if (posi == None)
    {
        df_move_deg = 0;
        df_move_power = 0;

        df_line_trace_power = 0;
    }

    Vector vec;
    vec = lineTrace(df_move_deg, df_move_power, df_line_trace_power);

    return vec;
}

//// ディフエンダーメイン ////
void playDefenderVer4(Defender::Mode mode)
{
    //// 自分ゴールと相手ゴールの確認
    goalPositionCheck(mode);

    //// フォーム決定
    DfForm df_form;
    LinePosition line_position = linePositionCheck();

    // 攻撃フォーム切り替え条件
    static bool my_defence_antei_condition = false; // 守備フォーム かつ ボールとゴールの線分上にいる時true
    static bool old_my_defence_antei_condition = false;

    static Timer my_defence_antei_condition_timer; // 守備フォーム かつ ボールとゴールの線分上にいる時間計測用
    if (old_my_defence_antei_condition == false && my_defence_antei_condition == true)
    {
        my_defence_antei_condition_timer.reset();
    }

    old_my_defence_antei_condition = my_defence_antei_condition; // 記録
    my_defence_antei_condition = false;                          // リセット

    // 攻撃フォーム取り消し条件
    static bool attack_form = false; // 現在ATTACKERフォームかどうか
    static bool old_attack_form = false;
    static Timer attacker_form_timer; // ATTAKCERフォーム継続時間

    static bool direct_line_trace_flag = true; // ダイレクトにライントレースするかどうか
    static Timer direct_line_trace_last_timer;

    if (attack_form == true && old_attack_form == false)
    {
        attacker_form_timer.reset();
        direct_line_trace_flag = true;
    }

    old_attack_form = attack_form; // 記録

    if (attack_form)
    {
        if (direct_line_trace_flag == true && fabsf(irY()) < 80)
        {
            direct_line_trace_flag = false;
            direct_line_trace_last_timer.reset();
        }

        // ボールが見えなかったら取り消し
        if (!irDetected())
        {
            attack_form = false;
        }
        // ラインに反応したらATTACKERフォーム取り消し
        else if (lineRingDetected() && direct_line_trace_flag == false && direct_line_trace_last_timer.msTime() > 2000)
        {
            attack_form = false;
        }
        // 5秒以上たったらATTACKERフォーム取り消し
        else if (attacker_form_timer.msTime() > 5000UL)
        {
            attack_form = false;
        }
    }

    // 代入
    if (attack_form)
    {
        if (direct_line_trace_flag == true)
        {
            df_form = DfForm::ATTACKER_DIRECT_LINETRACE; // ATTACKER_DIRECT_LINETRACEフォーム
        }
        else
        {
            df_form = DfForm::ATTACKER; // ATTACKERフォーム
        }
    }
    else if (isMyDefenceArea())
    {
        if (lineRingDetected())
        {
            if (old_my_defence_antei_condition && my_defence_antei_condition_timer.msTime() > 2500UL)
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

    if (df_form == DfForm::ATTACKER_DIRECT_LINETRACE)
    {
        float length = smoothShifter(80, 100, 0, 60, fabsf(irY()));
        Vector vec = dfLineTrace(line_position, irDeg(), length, 10);
        motorsVectorMove(&vec);
    }
    else if (df_form == DfForm::ATTACKER)
    {
        playAttacker((mode == Defender::Mode::YELLOWGOAL) ? Attacker::Mode::YELLOWGOAL : Attacker::Mode::BLUEGOAL);
    }
    else if (df_form == DfForm::DEFENCE)
    {
        static bool old_ir_detected = false;
        static Timer ir_last_detected_timer;
        if (!irDetected() && old_ir_detected)
        {
            ir_last_detected_timer.reset();
        }

        if (irDetected())
        {
            float defence_ir_deg = nearSessenDeg(defenceGoalDeg(), irDeg()); // 守備ゴールの±90度の方向のうち、ボールの方向に近い角度

            float defence_ir_y = irDis() * sinf(radians(diffDeg(defenceGoalDeg() + 180, irDeg()))); // y成分
            float defence_ir_x = irDis() * cosf(radians(diffDeg(defenceGoalDeg() + 180, irDeg()))); // x成分

            static MovementAverage y_ave(10);
            float smooth_defence_ir_y = y_ave.add(defence_ir_y);

            Vector vec;
            if (defence_ir_x > 0 && fabsf(defence_ir_y) < 100)
            {
                if (line_position == LinePosition::Yoko && fabsf(smooth_defence_ir_y) < 100)
                {
                    my_defence_antei_condition = true;
                }

                if (fabsf(defence_ir_y) < 80)
                {
                    vec = Vector(lineRingDeg(), 10);
                }
                else
                {
                    float length = smoothShifter(80, 100, 0, 90, fabsf(defence_ir_y));
                    vec = dfLineTrace(line_position, defence_ir_deg, length, 10);
                }
            }
            else
            {
                vec = dfLineTrace(line_position, defence_ir_deg, 90, 10);
            }
            motorsVectorMove(&vec);
        }
        else if (ir_last_detected_timer.everReset() && ir_last_detected_timer.msTime() < 2000)
        {
            Vector vec = Vector(lineRingDeg(), 10);
            motorsVectorMove(&vec);
        }
        else
        {
            // 定位置移動
            Vector vec;
            if (fabsf(defenceGoalDeg()) > 165)
            {
                vec = lineTrace(0, 0, 10);
            }
            else
            {
                float teiiti_deg = (defenceGoalDeg() > 0) ? defenceGoalDeg() - 90 : defenceGoalDeg() + 90;
                vec = dfLineTrace(line_position, teiiti_deg, 90, 10);
            }
            motorsVectorMove(&vec);
        }

        old_ir_detected = irDetected();
    }
    else if (df_form == DfForm::DEFENCE_LINE_HOSEI)
    {
        if (defenceGoalDis() < 60)
        {
            motorsMove(defenceGoalDeg() + 180, 80);
        }
        else
        {
            motorsMove(lineRingLastDeg(), 80);
        }
    }
    else // if (df_form == DfForm::KIKAN)
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
                    vec = lineTrace(90, 60, 10); // ライントレースする
                }
                else
                {
                    vec = lineTrace(-90, 60, 10); // ライントレースする
                }
                motorsVectorMove(&vec);
            }
            else
            {
                motorsMove(fieldDeg(), 80);
            }
        }
        else if (defenceGoalDetected())
        {
            // ゴールに向かう動き
            motorsMove(defenceGoalDeg(), 60);
        }
        else
        {
            motorsMove(180, 60); // 後ろに移動する
        }
    }
}