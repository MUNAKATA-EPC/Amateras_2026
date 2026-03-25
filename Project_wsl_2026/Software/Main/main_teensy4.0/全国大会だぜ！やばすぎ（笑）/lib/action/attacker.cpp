#include "attacker.hpp"

static PD pd_gyro(1.2, -0.7); // ジャイロ用のPD調節値
static PD pd_cam(1.2, -0.7);  // カメラ用のPD調節値

static Timer my_timer;
static Timer kicker_timer;

const float line_escape_power = 55.0f; // ライン反応時のモーターの強さ
const float line_trace_power = 75.0f;  // ライントレース時モーターの強さ
const float ir_max_power = 95.0f;      // モーターの強さ

const float yellow_goal_kyori = 91.0f; // キッカー判定用の黄色ゴール距離
const float blue_goal_kyori = 85.0f;   // キッカー判定用の青色ゴール距離

// 回り込み関数
Vector mawarikomi(float max_power, float ball_deg, float ball_dis)
{
    if (!irDetected())
        return Vector(0, 0.0f);

    float ir_x = cosf(radians(ball_deg)) * ball_dis;
    float ir_y = sinf(radians(ball_deg)) * ball_dis;

    Vector vec;

    if (catchSensor.read() == HIGH)
    {
        vec = Vector(0.0f, max_power);
    }
    else if (ir_x > 0.0f && fabsf(ir_y) < 140.0f)
    {
        // rad計算
        float dis_x = smoothShifter(60.0f, 140.0f, 65.0f, 280.0f, ir_y);
        // length計算
        float length_temp;
        if (fabsf(ir_y) < 60.0f)
        {
            length_temp = max_power;
        }
        else
        {
            // length_temp = smoothShifter(60.0f, 150.0f, 50.0f, 95.0f, ir_y);
            length_temp = max_power;
        }

        Vector vec_temp = Vector(dis_x, 0, ir_x, ir_y);
        vec_temp = vec_temp * length_temp / vec_temp.length();

        vec = vec_temp;
    }
    else
    {
        const float r = 360.0f;
        const float pi = (float)PI;

        float ir_rad = (float)radians(ball_deg);
        float ir_dis = ball_dis;

        float rad_temp;
        float length_temp = 85.0f;

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

    static float smooth_length = 60.0f;
    if (vec.length() >= smooth_length) // ゆっくり加速
    {
        smooth_length += 1.5f;
    }
    else
    {
        smooth_length -= 1.5f;
    }
    smooth_length = constrain(smooth_length, 0.0f, 95.0f);

    // Vector final_vec = Vector(vec.deg(), smooth_length);
    Vector final_vec = vec;

    return final_vec;
}

//// アタッカーメイン ////
void playAttacker(Attacker::Mode mode)
{
    //// ゴールの位置更新
    goalPositionCheckAtMode(mode);

    //// PD制御目標方向確定
    static Timer last_catch_timer;
    bool a = catchSensor.read() == HIGH;
    static bool b = false;
    if (!a && b)
    {
        last_catch_timer.reset();
    }
    b = a;

    if (mode == Attacker::Mode::GYRO)
    {
        motorsPdProcess(&pd_gyro, bnoDeg(), 0);
    }
    else // (mode == Attacker::Mode::YELLOWGOAL) || (mode == Attacker::Mode::BLUEGOAL)
    {
        if (attackGoalDetected())
        {
            int diff = fabsf(normalizeDeg(diffDeg(bnoDeg(), attackGoalDeg())));

            // if (catchSensor.read() == HIGH || fabsf(iry) <= 130)
            if (catchSensor.read() == HIGH || last_catch_timer.msTime() < 200)
            {
                if (!irDetected())
                {
                    motorsPdProcess(&pd_gyro, bnoDeg(), 0);
                }
                else
                {
                    if (diff > 40)
                    {
                        if (attackGoalDeg() < 0)
                            motorsPdProcess(&pd_gyro, bnoDeg(), 40);
                        else
                            motorsPdProcess(&pd_cam, bnoDeg(), -40);
                    }
                    else
                    {
                        motorsPdProcess(&pd_cam, attackGoalDeg(), 0);
                    }
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
    }

    //// ライン・ライントレース動作決定
    LinePosition current_line_position = linePositionCheck();
    if (current_line_position == LinePosition::None)
    {
        if (lineSideRightDetected())
        {
            if (lineSideLeftDetected())
            {
                current_line_position = LinePosition::Yoko;
            }
            else
            {
                current_line_position = LinePosition::Tate;
            }
        }
        else if (lineSideLeftDetected())
        {
            current_line_position = LinePosition::Tate;
        }
    }

    static bool line_trace_flag = false;
    static Timer line_trace_timer;

    enum LineForm
    {
        LineEscape,    // 普通の回避動作
        LineTraceYoko, // 横線上のトレース
        LineTraceTate, // 縦線上のトレース
        None
    };
    LineForm line_form = LineForm::None;

    static Timer line_timer;

    bool line_detected = (current_line_position != LinePosition::None);
    static bool old_line_detected = false;

    static int line_switching_count = 0;
    static LinePosition memory_last_line_position;

    // ラインフォームをとりあえず決定
    if (!line_trace_flag)
    {
        if (!line_detected)
        {
            if (old_line_detected)
            {
                line_timer.reset();
            }

            if (line_timer.msTime() < 80UL) // ラインから逃げる時間(普通の回避動作)
            {
                line_form = LineForm::LineEscape;
            }
            else
            {
                line_form = LineForm::None;
            }
        }
        else
        {
            line_form = LineForm::LineEscape;

            if (!old_line_detected)
            {
                const int line_switching_min_time = 300;  // ライントレースのカウントの時間(min)
                const int line_switching_max_time = 2000; // ライントレースのカウントの時間(max)

                if (line_timer.msTime() > line_switching_min_time && line_timer.msTime() < line_switching_max_time)
                {
                    line_switching_count++;
                }
                else
                {
                    line_switching_count = 1;
                }

                memory_last_line_position = current_line_position; // Tate or Yoko
            }
        }
    }

    old_line_detected = line_detected; // 記録

    // 2回以上反応でライントレースモードに移行する
    static bool old_line_trace_flag = false;
    if (line_switching_count > 1 && (memory_last_line_position == LinePosition::Tate || memory_last_line_position == LinePosition::Yoko))
    {
        line_trace_flag = true;

        if (!old_line_trace_flag)
        {
            line_trace_timer.reset();
        }
    }
    old_line_trace_flag = line_trace_flag;

    // ライントレースモード解除条件
    if (line_trace_flag)
    {
        if (memory_last_line_position == LinePosition::Yoko)
        {
            if (isMyAttackArea()) // 攻撃エリアでの誤反応を無くす
            {
                line_trace_flag = false;
                line_switching_count = 0;
            }

            if (!isMyDefenceArea()) // 守備エリアでの誤反応を無くす
            {
                line_trace_flag = false;
                line_switching_count = 0;
            }

            // if (current_line_position == LinePosition::Tate || current_line_position == LinePosition::Haji) // ラインポジションが変わったら解除
            // {
            //      line_trace_flag = false;
            //      line_switching_count = 0;
            // }

            float line_ring_deg = defenceGoalDeg();

            if (fabsf(diffDeg(line_ring_deg, irDeg())) > 90) // ライン方向とボール方向の差を見て解除
            {
                line_trace_flag = false;
                line_switching_count = 0;
            }
        }
        else // (memory_last_line_position == LinePosition::Tate)
        {
            int line_ring_deg = (fieldDeg() > 0) ? -90 : 90;

            if (fabsf(diffDeg(line_ring_deg, irDeg())) > 110) // ライン方向とボール方向の差を見て解除
            {
                line_trace_flag = false;
                line_switching_count = 0;
            }

            if (isMyAttackArea()) // 攻撃エリアでの誤反応を無くす
            {
                line_trace_flag = false;
                line_switching_count = 0;
            }

            // if (current_line_position == LinePosition::Yoko || current_line_position == LinePosition::Haji) // ラインポジションが変わったら解除
            // {
            //      line_trace_flag = false;
            //      line_switching_count = 0;
            // }
        }
    }

    // ラインフォームをライントレースモードとして書き換え
    if (line_trace_flag)
    {
        if (memory_last_line_position == LinePosition::Yoko)
        {
            line_form = LineForm::LineTraceYoko;
        }
        else
        {
            line_form = LineForm::LineTraceTate;
        }
    }

    Serial.print("posi:");
    Serial.print((current_line_position == LinePosition::Yoko) ? "Yoko" : (current_line_position == LinePosition::Tate) ? "Tate"
                                                                                                                        : "None");

    Serial.print(", sw_cnt:");
    Serial.print(line_switching_count);

    Serial.print(", last_posi:");
    Serial.print((memory_last_line_position == LinePosition::Yoko) ? "Yoko" : (memory_last_line_position == LinePosition::Tate) ? "Tate"
                                                                                                                                : "None");

    Serial.print(", line_form:");
    Serial.print((line_form == LineForm::LineEscape) ? "Escape" : (line_form == LineForm::LineTraceTate) ? "Tate"
                                                              : (line_form == LineForm::LineTraceYoko)   ? "Yoko"
                                                                                                         : "None");

    Serial.print(", line_timer:" + String(line_timer.msTime()));

    Serial.println(", linetrace_flag:" + String(line_trace_flag));

    //// 制御
    bool kick_signal = false; // キックしたいture

    if (line_form == LineForm::LineEscape)
    {
        if (irDetected())
        {
            Vector vec = mawarikomi(ir_max_power, irDeg(), irDis());
            if (fabsf(diffDeg(vec.deg(), fieldDeg())) < 90)
            {
                motorsMove(vec.deg(), line_escape_power);
            }
            else
            {
                motorsMove(fieldDeg(), line_escape_power);
            }
        }
        else
        {
            motorsMove(fieldDeg(), line_escape_power);
        }
    }
    else if (line_form == LineForm::LineTraceYoko)
    {
        Vector line_escape_vec = Vector(fieldDeg(), 0);
        Vector ir_follow_vec = (irDeg() > 0) ? Vector(90, 8) : Vector(-90, 8);
        Vector final_vec = line_escape_vec + ir_follow_vec;
        final_vec = final_vec * line_trace_power / final_vec.length();

        motorsVectorMove(&final_vec);
    }
    else if (line_form == LineForm::LineTraceTate)
    {
        if (lineRingDetected())
        {
            if (lineRingDis() > 60 && fabsf(diffDeg(lineRingDeg(), fieldDeg())) > 90)
            {
                if (catchSensor.read() == HIGH)
                {
                    kick_signal = true;
                }

                motorsMove(nearSessenDeg(lineRingDeg(), irDeg()), line_trace_power);
            }
            else
            {
                motorsMove(fieldDeg(), line_trace_power);
            }
        }
        else
        {
            if (catchSensor.read() == HIGH)
            {
                kick_signal = true;
                motorsMove(irDeg(), line_trace_power);
            }
            else
            {
                Vector vec = mawarikomi(line_trace_power, irDeg(), irDis());
                motorsVectorMove(&vec);
            }
        }
    }
    //// ボール回り込み
    else if (irDetected())
    {
        // if (goaldetected && goaldis < 80)
        // {
        //     int goal_to_ball_deg = -diffDeg(goaldeg, irDeg());
        //     Vector vec = mawarikomi(ir_max_power, goaldis, goal_to_ball_deg, irDis());
        //     vec = Vector(vec.deg() + goaldeg, vec.length());
        //     motorsVectorMove(&vec);
        // }
        // else
        // {
        //     Vector vec = mawarikomi(ir_max_power, goaldis, irDeg(), irDis());
        //     motorsVectorMove(&vec);
        // }

        // motorsMove(0,0);
        Vector vec = mawarikomi(ir_max_power, irDeg(), irDis());
        motorsVectorMove(&vec);
    }
    else
    {
        motorsPdMove();
    }

    //// キッカー制御
    static Timer catching_timer;
    static bool old_catch = false;
    bool current_catch = (catchSensor.read() == HIGH);
    if (current_catch)
    {
        if (!old_catch)
        {
            catching_timer.reset();
        }

        if (mode == Attacker::Mode::GYRO)
        {
            if (catching_timer.msTime() > 80)
            {
                kicker1.kick();
            }
        }
        else // (mode == Attacker::Mode::YELLOWGOAL) || (mode == Attacker::Mode::BLUEGOAL)
        {
            if (kick_signal) // ライントレース時のキック指示は問答無用でキック
            {
                kicker1.kick();
            }
            else
            {
                const int kick_at_goal_dis = (mode == Attacker::Mode::YELLOWGOAL) ? yellow_goal_kyori : blue_goal_kyori;

                if (attackGoalDis() <= kick_at_goal_dis)
                {
                    kicker1.kick();
                }
                else // if (attackGoalDis() > kick_at_goal_dis)
                {
                    if (catching_timer.msTime() > 400)
                    {
                        kicker1.kick();
                    }
                }
            }
        }
    }
    old_catch = current_catch; // 記録
}

// bno関係の関数
// bnoDeg();    bnoからの角度を取得　⚠リセットボタンが押されると自動的に0になるようになっている

// PD関係のコンストラクタ
// PD pd_gyro(P成分の係数, D成分の係数);     Pの係数を大きくすると戻る力が大きく、Dの係数を大きくすると戻る力が弱まる

// Motor関係の関数
// motorsPdProcess(&pd_gyro, bnoDeg(), 0);   絶対毎回呼び出すこと、PDの計算を行う
// motorsMove(deg,power);                    degの方向にpowerの力で動かす　⚠前:0、左:90、右:-90、後:180
// motorsPdMove();                           PD制御のみ行う

// IR関係の関数
// irDetected();    IRボールがあるかどうか取得      1か0
// irDeg();         IRボールの角度取得             -180~180       ⚠前:0、左:90、右:-90、後:180 ⚠ボールがないときは0xFF=255を返す
// irDis();         IRボールの距離取得             0.0 ~ 1023.0
// irVal();         IRボールの(1023.0-距離)取得    1023.0 ~ 0.0
// irX();           IRボールのX座標取得            0.0 ~ 1023.0   ⚠前がx軸、正方向
// irY();           IRボールのY座標取得            0.0 ~ 1023.0   ⚠左がy軸、正方向

// LINE関係の関数
// lineRingDetected();    LINEがあるかどうか取得      1か0
// lineRingDeg();         LINEの角度取得             -180~180       ⚠前:0、左:90、右:-90、後:180 ⚠ボールがないときは0xFF=255を返す
// lineRingDis();         LINEの距離取得             0.0 ~ 100.0
// lineRingX();           ILINEのX座標取得           0.0 ~ 1023.0   ⚠前がx軸、正方向
// lineRingY();           LINEのY座標取得            0.0 ~ 1023.0   ⚠左がy軸、正方向

// キャッチセンサーの関数
// catchSensor.read()  == HIGH or LOW

// キッカーの関数
// kicker1.kick();