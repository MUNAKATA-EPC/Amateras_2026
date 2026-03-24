#include "attacker.hpp"

// アタッカーのフォーム
enum AtForm
{
    LINE_ESCAPE,          // ラインから逃れるフォーム
    LINE_CONTINUE_ESCAPE, // ラインから逃れる動きを継続するフォーム
    LINE_TRACE_TATE,      // ライントレースするフォーム（コートの縦白線上でトレース）
    FOLLOW_BALL,          // 回り込むフォーム
    HOLD,                 // ボール捕獲時のゴールに向かうフォーム（LINE_TRACE_TURNフォームでない場合）
    LOSE_BALL             // ボールが見えず停滞するフォーム
};

// 接線
Vector sessen(float rad, float dis, float r, float length)
{
    const float pi = (float)PI;

    float rad_temp;
    float length_temp = length;

    if (r <= dis)
    {
        float alpha = asinf(r / dis);

        if (rad >= 0)
        {
            rad_temp = rad + alpha;
        }
        else // (ir_rad < 0)
        {
            rad_temp = rad - alpha;
        }
    }
    else // (r > ir_dis)
    {
        if (rad >= 0)
        {
            rad_temp = rad + pi / 2;
        }
        else // (ir_rad < 0)
        {
            rad_temp = rad - pi / 2;
        }
    }

    return Vector(degrees(rad_temp), length_temp);
}

// 回り込む関数
Vector mawarikomi(float ball_detected, float ball_deg, float ball_dis)
{
    if (!ball_detected)
        return Vector(0, 0.0f);

    float rad = (float)radians(ball_deg);
    float dis = ball_dis;
    float x = dis * cosf(radians(ball_deg));
    float y = dis * sinf(radians(ball_deg));

    Vector vec;

    if (dis > 580)
    {
        vec = Vector(degrees(rad), 95.0f);
    }
    else if (x > 0.0f && fabsf(y) < 120.0f)
    {
        if (catchSensor.read() == HIGH)
        {
            vec = Vector(0.0f, 95.0f);
        }
        else
        {
            // rad計算
            float dis_x = smoothShifter(60.0f, 90.0f, 50.0f, 300.0f, y);
            // length計算
            float length_temp;
            if (fabsf(y) < 55.0f)
            {
                length_temp = 95.0f;
            }
            else
            {
                length_temp = smoothShifter(60.0f, 90.0f, 50.0f, 95.0f, y);
            }

            Vector vec_temp = Vector(dis_x, 0, x, y);
            vec_temp = vec_temp * length_temp / vec_temp.length();

            vec = vec_temp;
        }
    }
    else
    {
        vec = sessen(rad, dis, 300, 95.0f);
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
    goalPositionCheckAtMode(mode);

    //// フォーム決定
    static AtForm current_at_form = AtForm::LOSE_BALL;
    static AtForm old_at_form;

    LinePosition line_position = linePositionCheck(); // ラインによる自己位置推定

    // ライン反応終了後LINE_CONTINUE_ESCAPEフォームに移行するためのタイマー 兼 どのくらいに周期でラインが反応しているのか確認するためのタイマー
    static Timer line_timer;

    // 600ms周期で同じポジションを示し続ければcountをアップさせる（LINETRACE用）
    const int switching_line_position_time = 2000;
    static int switching_line_position_count = 0;
    static LinePosition switching_line_position_memory;

    // LINETRACEをすべきか決定
    static bool line_trace_flag = false; // これが一度trueになるとfalseにするまでずっとライントレースをし続ける
    if (irDetected() && !line_trace_flag && switching_line_position_count >= 2)
    {
        if (switching_line_position_memory == LinePosition::Tate)
        {
            line_trace_flag = true;

            current_at_form = AtForm::LINE_TRACE_TATE;
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

        if (lineRingDetected() && switching_line_position_memory != line_position) // 今のラインの状況と異なるならトレースをやめる
        {
            line_trace_flag = false;
            switching_line_position_count = 0; // カウントリセット
        }
        else
        {
            Vector vec_temp = mawarikomi(irDetected(), irDeg(), irDis());
            if (current_at_form == AtForm::LINE_TRACE_TATE)
            {
                if (line_tracing_timer.msTime() > 5000) // 5秒トレースしたらトレースをやめる
                {
                    line_trace_flag = false;
                    switching_line_position_count = 0; // カウントリセット
                }

                // コートの右にいて回り込む方向が左 または コートの左にいて回り込む方向が右のときトレースをやめる
                if ((fieldDeg() > 0 && (vec_temp.deg() > 20 && vec_temp.deg() < 160)) ||
                    (fieldDeg() < 0 && (vec_temp.deg() < -20 && vec_temp.deg() > -160)))
                {
                    line_trace_flag = false;
                    switching_line_position_count = 0; // カウントリセット
                }
            }
        }
    }
    // ライントレースフラグが立っていないときのフォーム
    else // if (!line_trace_flag)
    {
        static LinePosition last_line_position;

        if (lineRingDetected())
        {
            current_at_form = AtForm::LINE_ESCAPE;
            last_line_position = line_position;
        }
        else if (lineSideLeftDetected())
        {
            if (lineSideRightDetected())
            {
                last_line_position = LinePosition::Yoko;
            }
            else
            {
                last_line_position = LinePosition::Tate;
            }

            current_at_form = AtForm::LINE_ESCAPE;
        }
        else if (lineSideRightDetected())
        {
            last_line_position = LinePosition::Tate;

            current_at_form = AtForm::LINE_ESCAPE;
        }
        else if ((attackGoalDetected() && attackGoalDis() < 60) || (defenceGoalDetected() && defenceGoalDis() < 60))
        {
            last_line_position = LinePosition::Yoko;

            current_at_form = AtForm::LINE_ESCAPE;
        }
        else if (old_at_form == AtForm::LINE_ESCAPE)
        {
            if (line_timer.everReset() && line_timer.msTime() < switching_line_position_time)
            {
                if (last_line_position == switching_line_position_memory)
                {
                    switching_line_position_count++;
                }
                else
                {
                    switching_line_position_memory = last_line_position; // 新たなポジションとして更新
                    switching_line_position_count = 0;
                }
            }
            else
            {
                switching_line_position_memory = last_line_position; // 新たなポジションとして更新
                switching_line_position_count = 0;
            }

            line_timer.reset();
            current_at_form = AtForm::LINE_CONTINUE_ESCAPE;
        }
        else if (line_timer.everReset() && line_timer.msTime() < 100UL)
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

    old_line_trace_flag = line_trace_flag; // 記録

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
        if (lineRingDetected())
        {
            move_vec = lineTrace(irDeg(), 60, 10);
        }
        else
        {
            move_vec = mawarikomi(irDetected(), irDeg(), irDis());
        }
    }
    else if (current_at_form == AtForm::FOLLOW_BALL)
    {
        if (false) // (attackGoalDetected() && attackGoalDis() < 95)
        {
            float k = smoothShifter(30, 40, 0, 1, fabsf(yellowGoalDeg()));
            if (fabsf(yellowGoalDeg()) > 40)
            {
                k = smoothShifter(40, 50, 1, 0, fabsf(yellowGoalDeg()));
            }
            float goal_to_ir_deg = -diffDeg(attackGoalDeg() * k, irDeg());
            Vector vec = mawarikomi(irDetected(), goal_to_ir_deg, irDis());
            move_vec = Vector(vec.deg() + attackGoalDeg(), vec.length());
        }
        else
        {
            move_vec = mawarikomi(irDetected(), irDeg(), irDis());
        }
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
    static PD pd_gyro(0.50f, -3.0f);
    static PD pd_cam(0.5f, -3.0f);
    static PD pd_zure_gyro(1.5f, -3.0f);

    if (current_at_form == AtForm::HOLD)
    {
        static Timer hold_timer;
        if (old_at_form != AtForm::HOLD)
        {
            hold_timer.reset();
        }

        if (mode == Attacker::Mode::GYRO)
        {
            if (hold_timer.msTime() > 500) // 500msホールドし続けると機体をずらす
            {
                if (fieldDeg() > 0) // コート右にいる
                {
                    motorsPdProcess(&pd_zure_gyro, bnoDeg(), -45);
                }
                else
                {
                    motorsPdProcess(&pd_zure_gyro, bnoDeg(), 45);
                }
            }
            else
            {
                motorsPdProcess(&pd_gyro, bnoDeg(), 0);
            }

            kicker1.kick();
        }
        else
        {
            if (pd_target_deg != 0)
            {
                // LINE_TRACE_YOKO_AND_TURNフォーム時のみpd_target_degの方向に向く（回転）
                motorsPdProcess(&pd_cam, bnoDeg(), pd_target_deg);
            }
            else
            {
                // 基本正面を向く
                if (attackGoalDetected())
                {
                    motorsPdProcess(&pd_cam, attackGoalDeg(), 0);
                }
                else
                {
                    if (hold_timer.msTime() > 500) // 500msホールドし続けると機体をずらす
                    {
                        if (fieldDeg() > 0) // コート右にいる
                        {
                            motorsPdProcess(&pd_zure_gyro, bnoDeg(), -45);
                        }
                        else
                        {
                            motorsPdProcess(&pd_zure_gyro, bnoDeg(), 45);
                        }
                    }
                    else
                    {
                        motorsPdProcess(&pd_gyro, bnoDeg(), 0);
                    }
                }
            }

            if (fabsf(attackGoalDeg()) < 80)
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
            motorsPdProcess(&pd_cam, bnoDeg(), pd_target_deg);
        }
        else
        {
            // 基本正面を向く
            motorsPdProcess(&pd_gyro, bnoDeg(), 0);
        }
    }

    motorsVectorMove(&move_vec);

    old_at_form = current_at_form; // 記録
}