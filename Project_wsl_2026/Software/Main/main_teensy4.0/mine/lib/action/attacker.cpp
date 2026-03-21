#include "attacker.hpp"

// アタッカーのフォーム
namespace AtForm
{
    enum CamMode
    {
        LINE_ESCAPE,          // ラインから逃れるフォーム
        LINE_CONTINUE_ESCAPE, // ラインから逃れる動きを継続するフォーム
        LINE_TRACE,           // ライントレースするフォーム
        MAWARIKOMI,           // 回り込むフォーム
        HOLD,                 // ボール捕獲時のゴールに向かうフォーム
        LOSE_BALL             // ボールが見えず停滞するフォーム
    };

    enum GyroMode
    {
        LINE_ESCAPE,          // ラインから逃れるフォーム
        LINE_CONTINUE_ESCAPE, // ラインから逃れる動きを継続するフォーム
        MAWARIKOMI,           // 回り込むフォーム
        HOLD,                 // ボール捕獲時の直進するフォーム
        LOSE_BALL             // ボールが見えず停滞するフォーム
    };
}

// 回り込みをする関数
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

// ジャイロで攻撃
void attackWithGyro(AtForm::GyroMode at_form)
{
    if (at_form == AtForm::GyroMode::LINE_ESCAPE)
    {
        motorsMove(lineRingFirstDeg() + 180, 60);
    }
    else if (at_form == AtForm::GyroMode::LINE_CONTINUE_ESCAPE)
    {
        motorsMove(lineRingLastDeg() + 180, 60);
    }
    else if (at_form == AtForm::GyroMode::MAWARIKOMI)
    {
        Vector vec = mawarikomi();
        motorsVectorMove(&vec);
    }
    else if (at_form == AtForm::GyroMode::HOLD)
    {
        motorsMove(0, 95);
    }
    else // if (at_form == AtForm::GyroMode::LOSE_BALL)
    {
        motorsPdMove();
    }
}

// カメラで攻撃
void attackWithCam(AtForm::CamMode at_form)
{
    if (at_form == AtForm::CamMode::LINE_ESCAPE || at_form == AtForm::CamMode::LINE_CONTINUE_ESCAPE)
    {
        motorsMove(fieldDeg(), 60);
    }
    else if (at_form == AtForm::CamMode::LINE_TRACE)
    {
    }
    else if (at_form == AtForm::CamMode::MAWARIKOMI)
    {
        Vector vec = mawarikomi();
        motorsVectorMove(&vec);
    }
    else if (at_form == AtForm::CamMode::HOLD)
    {
        motorsMove(0, 95);
    }
    else // if (at_form == AtForm::CamMode::LOSE_BALL)
    {
        motorsPdMove();
    }
}

//// アタッカーメイン ////
void playAttacker(Attacker::Mode mode)
{
    static Timer line_timer;

    if (mode == Attacker::Mode::GYRO)
    {
        AtForm::GyroMode current_at_form;
        static AtForm::GyroMode old_at_form;

        //// フォーム決定
        if (lineRingDetected() || lineSideDetected())
        {
            current_at_form = AtForm::LINE_ESCAPE;
        }
        else if (old_at_form == AtForm::LINE_ESCAPE)
        {
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
                    current_at_form = AtForm::MAWARIKOMI;
                }
                else
                {
                    current_at_form = AtForm::LOSE_BALL;
                }
            }
        }

        //// 攻撃制御
        // pd・キッカー
        motorsPdProcess(&pd_gyro, bnoDeg(), 0);

        if (current_at_form == AtForm::HOLD)
        {
            kicker1.kick();
        }

        // 移動
        attackWithGyro(current_at_form);

        old_at_form = current_at_form; // 記録
    }
    else
    {
        AtForm::CamMode current_at_form;
        static AtForm::CamMode old_at_form;

        //// 自分ゴールと相手ゴールの確認
        goalPositionCheck(mode);

        //// フォーム決定
        if (lineRingDetected() || lineSideDetected())
        {
            current_at_form = AtForm::LINE_ESCAPE;
        }
        else if (old_at_form == AtForm::LINE_ESCAPE)
        {
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
                    current_at_form = AtForm::MAWARIKOMI;
                }
                else
                {
                    current_at_form = AtForm::LOSE_BALL;
                }
            }
        }

        //// 攻撃制御
        // pd・キッカー
        if (current_at_form == AtForm::HOLD)
        {
            if (attackGoalDetected())
            {
                motorsPdProcess(&pd_cam, attackGoalDeg(), 0);

                if (fabsf(attackGoalDeg()) < 20)
                {
                    kicker1.kick();
                }
            }
            else
            {
                motorsPdProcess(&pd_gyro, bnoDeg(), 0);
            }
        }
        else
        {
            motorsPdProcess(&pd_gyro, bnoDeg(), 0);
        }

        // 移動
        attackWithCam(current_at_form);

        old_at_form = current_at_form; // 記録
    }
}