#include "attacker.hpp"

// アタッカーのフォーム
enum AtForm
{
    LINE_ESCAPE,              // ラインから逃れるフォーム
    LINE_CONTINUE_ESCAPE,     // ラインから逃れる動きを継続するフォーム
    LINE_TRACE_TATE,          // ライントレースするフォーム（コートの縦白線上でトレース）
    LINE_TRACE_YOKO,          // ライントレースするフォーム（自分・相手ゴール付近の横白線上でトレース）
    LINE_TRACE_YOKO_AND_TURN, // ライントレースするフォーム（相手ゴールのゴール外の横白線で回転）
    FOLLOW_BALL,              // 回り込むフォーム
    HOLD,                     // ボール捕獲時のゴールに向かうフォーム（LINE_TRACE_TURNフォームでない場合）
    LOSE_BALL                 // ボールが見えず停滞するフォーム
};

// 回り込む関数
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
    else if (ir_x > 0.0f && fabsf(ir_y) < 95.0f)
    {
        if (catchSensor.read() == HIGH)
        {
            vec = Vector(0.0f, 95.0f);
        }
        else
        {
            // rad計算
            float dis_x = smoothShifter(45.0f, 100.0f, 60.0f, 280.0f, ir_y);
            // length計算
            float length_temp;
            if (fabsf(ir_y) < 45.0f)
            {
                length_temp = 95.0f;
            }
            else
            {
                length_temp = smoothShifter(45.0f, 140.0f, 60.0f, 95.0f, ir_y);
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

    // static float smooth_length = 60.0f;

    // if (vec.length() >= smooth_length)
    // {
    //     smooth_length += 1.5f;
    // }
    // else
    // {
    //     smooth_length -= 1.5f;
    // }

    // smooth_length = constrain(smooth_length, 0.0f, 95.0f);

    // Vector final_vec = Vector(vec.deg(), smooth_length);
    // return final_vec;

    return vec;
}

//// アタッカーメイン ////
void playAttacker(Attacker::Mode mode)
{
    //// 自分ゴールと相手ゴールの確認
    goalPositionCheck(mode);

    //// フォーム決定
    static AtForm current_at_form = AtForm::LOSE_BALL;
    static AtForm old_at_form;

    LinePosition line_position = linePositionCheck(); // ラインによる自己位置推定

    // ライン反応終了後LINE_CONTINUE_ESCAPEフォームに移行するためのタイマー 兼 どのくらいに周期でラインが反応しているのか確認するためのタイマー
    static Timer line_timer;

    // 600ms周期で同じポジションを示し続ければcountをアップさせる（LINETRACE用）
    const int switching_line_position_time = 600;
    static int switching_line_position_count = 0;
    static LinePosition switching_line_position_memory;

    // LINETRACEをすべきか決定
    static bool line_trace_flag = false; // これが一度trueになるとfalseにするまでずっとライントレースをし続ける
    if (irDetected() && !line_trace_flag && switching_line_position_count > 4)
    {
        if (switching_line_position_memory == LinePosition::Tate)
        {
            line_trace_flag = true;

            current_at_form = AtForm::LINE_TRACE_TATE;
        }
        else
        {
            if (isMyDefenceArea() && switching_line_position_memory == LinePosition::Yoko)
            {
                // オウンゴールする可能性が低い位置にボールがいるとき
                if (fabsf(diffDeg(defenceGoalDeg(), irDeg())) > 45)
                {
                    line_trace_flag = true;

                    current_at_form = AtForm::LINE_TRACE_YOKO;
                }
            }
            else if (isMyAttackArea() && switching_line_position_memory == LinePosition::Yoko)
            {
                line_trace_flag = true;

                current_at_form = AtForm::LINE_TRACE_YOKO;
            }
            else
            {
                if (fabsf(fieldDeg()) > 90) // コート前にいる
                {
                    if (switching_line_position_memory == LinePosition::Yoko || switching_line_position_memory == LinePosition::Haji)
                    {
                        line_trace_flag = true;

                        current_at_form = AtForm::LINE_TRACE_YOKO_AND_TURN;
                    }
                }
            }
        }
    }

    // LINETRACEを取り消すべきか決定
    static bool old_line_trace_flag = false;
    static Timer line_tracing_timer;

    if (line_trace_flag)
    {
        if (!old_line_trace_flag && line_trace_flag)
        {
            line_tracing_timer.reset();
        }

        if (switching_line_position_memory != line_position) // 今のラインの状況と異なるならトレースをやめる
        {
            line_trace_flag = false;
            switching_line_position_count = 0; // カウントリセット
        }
        else
        {
            Vector vec_temp = mawarikomi();

            if (current_at_form == AtForm::LINE_TRACE_TATE)
            {
                if (line_tracing_timer.msTime() > 5000) // 5秒トレースしたらトレースをやめる
                {
                    line_trace_flag = false;
                    switching_line_position_count = 0; // カウントリセット
                }

                // コートの右にいて回り込む方向が左 または コートの左にいて回り込む方向が右のときトレースをやめる
                if ((fieldDeg() > 0 && vec_temp.deg() > 0) || (fieldDeg() < 0 && vec_temp.deg() < 0))
                {
                    line_trace_flag = false;
                    switching_line_position_count = 0; // カウントリセット
                }
            }
            else if (current_at_form == AtForm::LINE_TRACE_YOKO)
            {
                if (line_tracing_timer.msTime() > 5000) // 5秒トレースしたらトレースをやめる
                {
                    line_trace_flag = false;
                    switching_line_position_count = 0; // カウントリセット
                }

                if (isMyAttackArea() && fabsf(vec_temp.deg()) > 90) // 相手ゴール付近にいて後ろに行きたいならトレースをやめる
                {
                    line_trace_flag = false;
                    switching_line_position_count = 0; // カウントリセット
                }

                if (isMyDefenceArea() && fabsf(vec_temp.deg()) < 90) // 自分ゴール付近にいて前に行きたいならトレースをやめる
                {
                    line_trace_flag = false;
                    switching_line_position_count = 0; // カウントリセット
                }
            }
            else // if (current_at_form == AtForm::LINE_TRACE_YOKO_AND_TURN)
            {
                if (line_tracing_timer.msTime() > 5000) // 5秒トレースしたらトレースをやめる
                {
                    line_trace_flag = false;
                    switching_line_position_count = 0; // カウントリセット
                }

                if (fabsf(vec_temp.deg()) > 90) // 後ろに行きたいならトレースをやめる
                {
                    line_trace_flag = false;
                    switching_line_position_count = 0; // カウントリセット
                }
            }
        }
    }

    old_line_trace_flag = line_trace_flag;

    // ライントレースフラグが立っていないときのフォーム
    if (!line_trace_flag)
    {
        if (lineRingDetected() || lineSideDetected())
        {
            current_at_form = AtForm::LINE_ESCAPE;
        }
        else if (old_at_form == AtForm::LINE_ESCAPE)
        {
            if (line_timer.everReset() && line_timer.msTime() < switching_line_position_time)
            {
                if (line_position == switching_line_position_memory)
                {
                    switching_line_position_count++;
                }
                else
                {
                    switching_line_position_memory = line_position; // 新たなポジションとして更新
                    switching_line_position_count = 0;
                }
            }
            else
            {
                switching_line_position_memory = line_position; // 新たなポジションとして更新
                switching_line_position_count = 0;
            }

            line_timer.reset();
            current_at_form = AtForm::LINE_CONTINUE_ESCAPE;
        }
        else if (line_timer.everReset() && line_timer.msTime() < 50UL)
        {
            current_at_form = AtForm::LINE_CONTINUE_ESCAPE;
        }
        else
        {
            if (catchSensor.read() == HIGH)
            {
                current_at_form = AtForm::HOLD;
            }
            else
            {
                if (irDetected())
                {
                    current_at_form = AtForm::FOLLOW_BALL;
                }
                else
                {
                    current_at_form = AtForm::LOSE_BALL;
                }
            }
        }
    }

    //// 攻撃制御
    // 移動方向決定
    int pd_target_deg = 0; // 基本0度（LINE_TRACE_YOKO_AND_TURNフォーム時にtarget_degをずらすことで機体を回転させる）

    Vector move_vec;
    if (current_at_form == AtForm::LINE_ESCAPE)
    {
        move_vec = Vector(fieldDeg(), 60);
    }
    else if (current_at_form == AtForm::LINE_CONTINUE_ESCAPE)
    {
        move_vec = Vector(fieldDeg(), 60);
    }
    else if (current_at_form == AtForm::LINE_TRACE_TATE)
    {
        if (fabsf(irDeg()) < 90) // 前にボールがある
        {
            move_vec = lineTrace(0, 60, 10);
        }
        else // 後ろにボールがある
        {
            move_vec = lineTrace(180, 60, 10);
        }
    }
    else if (current_at_form == AtForm::LINE_TRACE_YOKO)
    {
        if (irDeg() > 0) // 左にボールがある
        {
            move_vec = lineTrace(90, 60, 10);
        }
        else // 右にボールがある
        {
            move_vec = lineTrace(-90, 60, 10);
        }
    }
    else if (current_at_form == AtForm::LINE_TRACE_YOKO_AND_TURN)
    {
        static bool turning_flag = false;
        static int old_pd_target_deg;
        static Timer turning_timer;

        if (old_at_form != current_at_form)
        {
            turning_flag = false;
        }

        if (turning_flag)
        {
            if (turning_timer.msTime() > 200) // 200msで回転終了させる
            {
                turning_flag = false;
            }
            else
            {
                pd_target_deg = old_pd_target_deg; // ±45度で固定させておく
            }
        }
        else
        {
            if (irDeg() > 20) // 左にボールがある かつ 今回転中じゃない
            {
                move_vec = lineTrace(90, 60, 10);
            }
            else if (irDeg() < -20) // 右にボールがある かつ 今回転中じゃない
            {
                move_vec = lineTrace(-90, 60, 10);
            }
            else
            {
                move_vec = Vector(lineRingDeg(), 10);

                if (fieldDeg() > 0) // コート右側
                {
                    pd_target_deg = 45; // 回転方向を変える
                }
                else // コート左側
                {
                    pd_target_deg = -45; // 回転方向を変える
                }

                turning_flag = true;
                turning_timer.reset();
            }
        }

        old_pd_target_deg = pd_target_deg; // 記録
    }
    else if (current_at_form == AtForm::FOLLOW_BALL)
    {
        move_vec = mawarikomi();
    }
    else if (current_at_form == AtForm::HOLD)
    {
        move_vec = Vector(0, 95);
    }
    else // if (current_at_form == AtForm::LOSE_BALL)
    {
        move_vec = Vector(0, 0);
    }

    // pd・キッカー
    if (current_at_form == AtForm::HOLD)
    {
        if (mode == Attacker::Mode::GYRO)
        {
            motorsPdProcess(&pd_gyro, bnoDeg(), 0);

            kicker1.kick();
        }
        else
        {
            if (pd_target_deg != 0)
            {
                // LINE_TRACE_YOKO_AND_TURNフォーム時のみpd_target_degの方向に向く（回転）
                motorsPdProcess(&pd_cam, attackGoalDeg(), pd_target_deg);
            }
            else
            {
                // 基本正面を向く
                motorsPdProcess(&pd_cam, attackGoalDeg(), 0);
            }

            if (fabsf(attackGoalDeg()) < 20)
            {
                kicker1.kick();
            }
        }
    }
    else
    {
        if (pd_target_deg != 0)
        {
            // LINE_TRACE_YOKO_AND_TURNフォーム時のみpd_target_degの方向に向く（回転）
            motorsPdProcess(&pd_cam, attackGoalDeg(), pd_target_deg);
        }
        else
        {
            // 基本正面を向く
            motorsPdProcess(&pd_cam, attackGoalDeg(), 0);
        }
    }

    motorsVectorMove(&move_vec);

    old_at_form = current_at_form; // 記録
}