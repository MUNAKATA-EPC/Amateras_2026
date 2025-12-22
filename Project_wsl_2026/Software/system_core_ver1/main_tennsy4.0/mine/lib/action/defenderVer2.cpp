#include "defenderVer2.hpp"

// 攻撃ゴールの位置
static bool attack_goal_detected;
static int attack_goal_deg;
static float attack_goal_dis;
// 守備ゴールの位置
static bool defence_goal_detected;
static int defence_goal_deg;
static float defence_goal_dis;

// ATTACK処理に使う変数
static int now_ir_deg = 0, old_ir_deg = 0;
static bool old_ir_keep_deg_flag = false;
static bool ir_keep_deg_flag = false, attack_flag = false;
static Timer ir_keep_deg_timer;
static Timer attacking_timer;

// キッカー処理用
static Timer kick_timer;
static bool old_catch = false;

void playDefender(Defender::Mode mode)
{
    //// 角度更新 ////
    if (mode == Defender::Mode::YELLOWGOAL)
    {
        attack_goal_detected = yellowGoalDetected();
        attack_goal_deg = yellowGoalDeg();
        attack_goal_dis = yellowGoalDis();

        defence_goal_detected = blueGoalDetected();
        defence_goal_deg = blueGoalDeg();
        defence_goal_dis = blueGoalDis();
    }
    else /*if (mode == Defender::Mode::BLUEGOAL)*/
    {
        defence_goal_detected = yellowGoalDetected();
        defence_goal_deg = yellowGoalDeg();
        defence_goal_dis = yellowGoalDis();

        attack_goal_detected = blueGoalDetected();
        attack_goal_deg = blueGoalDeg();
        attack_goal_dis = blueGoalDis();
    }

    //// キッカー処理 ////
    bool now_catch = (catchSensor.read() == HIGH);

    if (now_catch == true && old_catch == false)
    {
        kick_timer.reset();
    }

    if (now_catch && kick_timer.everReset() && kick_timer.msTime() >= 3000UL)
    {
        kicker1.kick(true);
    }
    else
    {
        kicker1.kick(false);
    }

    old_catch = now_catch;

    //// ATTACK処理 ////
    now_ir_deg = irDeg(); // 今のirの角度更新

    bool is_ir_keep =
        // 厳しい条件分岐でアタッカーに移行する
        irDetected() &&
        irDis() <= 70 &&
        abs(diffDeg(now_ir_deg, old_ir_deg)) <= 15 &&
        (defence_goal_detected && defence_goal_dis <= 75) &&
        !(catchSensor.read() == HIGH);

    if (is_ir_keep)
    {
        if (old_ir_keep_deg_flag == false) // 昔は一定ではなかったなら
        {
            ir_keep_deg_timer.reset();
            ir_keep_deg_flag = true;
        }
        old_ir_keep_deg_flag = true;

        if (ir_keep_deg_flag == true && ir_keep_deg_timer.msTime() >= 3000UL) // 5秒以上もボールが一定の角度にあるなら
        {
            attacking_timer.reset(); // アタッカータイマー開始
            attack_flag = true;
        }
    }
    else
    {
        old_ir_keep_deg_flag = false;
        ir_keep_deg_flag = false;
    }

    if (attack_flag == true && (!irDetected() || (attacking_timer.msTime() > 1000UL && lineRingDetected()))) // アタックモードに移行して1秒経った後ラインが反応したら戻る
    {
        attack_flag = false;
    }

    if (attack_flag == true && attacking_timer.msTime() <= 3000UL) // 3ms間アタッカーをする
    {
        if (attacking_timer.msTime() <= 500UL && (irDeg() >= -120 && irDeg() <= 120)) // とりあえずボールの方向へ行く
        {
            motorsPdProcess(&pd_gyro, bnoDeg(), 0);
            motorsMove(irDeg(), ATTACK_IR_FOLLOW_POWER);
        }
        else
        {
            playAttacker((mode == Defender::Mode::YELLOWGOAL) ? Attacker::Mode::YELLOWGOAL : Attacker::Mode::BLUEGOAL);
        }
        return;
    }
    else
    {
        attack_flag = false;
    }

    old_ir_deg = now_ir_deg; // 昔の角度記録

    //// DEFENCE処理 ////
    motorsPdProcess(&pd_gyro, bnoDeg(), 0); // PD更新制御

    if (defence_goal_detected && ((mode == Defender::Mode::YELLOWGOAL) ? (defence_goal_dis < DEFENCE_BLUE_GOAL_DIS) : (defence_goal_dis < DEFENCE_YELLOW_GOAL_DIS)))
    {
        if (lineRingDetected() &&
            (defence_goal_deg > ((mode == Defender::Mode::YELLOWGOAL) ? SAKAIME_BLUE_GOAL_DEG_MAX : SAKAIME_YELLOW_GOAL_DEG_MAX) ||
             defence_goal_deg < ((mode == Defender::Mode::YELLOWGOAL) ? SAKAIME_BLUE_GOAL_DEG_MIN : SAKAIME_YELLOW_GOAL_DEG_MIN))) // (SAKAIME処理)
        {
            // ボールの方向を決める
            int defence_ir_deg;
            if (abs(defence_goal_deg) >= ((mode == Defender::Mode::YELLOWGOAL) ? DEFENCE_BLUE_GOAL_NORMAL_DEG : DEFENCE_YELLOW_GOAL_NORMAL_DEG))
            {
                defence_ir_deg = irDeg();
            }
            else
            {
                int target_deg = normalizeDeg(defence_goal_deg + 180); // 対角線で守る
                defence_ir_deg = normalizeDeg(irDeg() - target_deg);
            }

            // ボールを守りたいベクトル
            Vector ir_defence_vec;
            Position posi = linePositionCheck();
            if (posi == Position::Yoko_line)
            {
                if (defence_ir_deg > 0) // 左にある
                {
                    ir_defence_vec = Vector(90, DEFENCE_YOKO_IR_FOLLOW_POWER_MAX);
                }
                else // 右にある
                {
                    ir_defence_vec = Vector(-90, DEFENCE_YOKO_IR_FOLLOW_POWER_MAX);
                }
            }
            else if (posi == Position::Kado_line)
            {
                int ir_near_sessen_deg = nearSeesenDeg(lineRingDeg(), defence_ir_deg);

                ir_defence_vec = Vector(ir_near_sessen_deg, DEFENCE_KADO_IR_FOLLOW_POWER_MAX);
            }
            else /*if (posi == Position::Tate_line)*/
            {
                if (abs(defence_ir_deg) < 90) // 前にある
                {
                    ir_defence_vec = Vector(0, DEFENCE_TATE_IR_FOLLOW_POWER_MAX);
                }
                else // 後にある
                {
                    ir_defence_vec = Vector(-180, DEFENCE_TATE_IR_FOLLOW_POWER_MAX);
                }
            }

            float defence_ir_y = irDis() * sinf(radians(defence_ir_deg)); // y方向成分
            if (defence_ir_y >= DEFENCE_IR_FRONT_Y_MIN &&
                defence_ir_y <= -DEFENCE_IR_FRONT_Y_MAX) // 一定の値の中に入るとボールを追わなくなる (停止)
            {
                ir_defence_vec = Vector(0, 0.0f);
            }

            // ライントレースするベクトル
            Vector line_trace_vec = Vector(lineRingDeg(), lineRingDis() * DEFENCE_LINE_TRACE_POWER_MAX / 100.0f);

            // 最終ベクトル
            Vector final_defence_vec = ir_defence_vec + line_trace_vec;

            motorsVectorMove(&final_defence_vec);
        }
        else
        {
            // MODORU処理
            Vector line_modoru_vec; // ラインに戻りたいベクトル
            if (defence_goal_dis < ((mode == Defender::Mode::YELLOWGOAL) ? MODORU_BLUE_GOAL_DIS : MODORU_YELLOW_GOAL_DIS))
            {
                line_modoru_vec = Vector(defence_goal_deg + 180, MODORU_POWER);
            }
            else
            {
                line_modoru_vec = Vector(defence_goal_deg, MODORU_POWER);
            }

            Vector ir_follow_vec; // ボール追いたいベクトル
            if (irDetected())
            {
                if (irDeg() > 0) // 左にある
                {
                    ir_follow_vec = Vector(90, MODORU_IR_FOLLOW_POWER);
                }
                else // 右にある
                {
                    ir_follow_vec = Vector(-90, MODORU_IR_FOLLOW_POWER);
                }
            }

            Vector final_modoru_vec = line_modoru_vec;               // 最終ベクトル
            if ((line_modoru_vec + ir_follow_vec).length() >= 20.0f) // 力同士が相殺されないのなら足し合わせる
            {
                final_modoru_vec = final_modoru_vec + ir_follow_vec;
            }

            motorsVectorMove(&final_modoru_vec);
        }
    }
    // TEIITI処理
    else if (lineRingDetected())
    {
        motorsMove(fieldDeg(), TEIITI_LINE_ESCAPE_POWER);
    }
    else if (defence_goal_detected)
    {
        motorsMove(defence_goal_deg, TEIITI_POWER);
    }
    else
    {
        if (fieldDetected())
        {
            if (abs(fieldDeg()) >= 90) // コートの後ろにいる
            {
                if (fieldDeg() > 0) // コートの右にいる
                {
                    motorsMove(145, TEIITI_POWER);
                }
                else // コートの左にいる
                {
                    motorsMove(-145, TEIITI_POWER);
                }
            }
            else // コートの前にいる
            {
                motorsMove(fieldDeg(), TEIITI_POWER);
            }
        }
        else
        {
            motorsMove(180, TEIITI_POWER);
        }
    }
}