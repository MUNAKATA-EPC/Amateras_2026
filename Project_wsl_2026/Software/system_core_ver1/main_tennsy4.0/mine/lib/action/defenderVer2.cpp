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

//// ディフェンスメイン ////
void playDefenderVer2(Defender::Mode mode)
{
    LinePosi posi = linePositionCheck(); // ラインによる自己位置推定
    /*if (posi == LinePosi::Kado_line)
    {
        Serial.println("kado");
    }
    else if (posi == LinePosi::Tate_line)
    {
        Serial.println("tate");
    }
    else if (posi == LinePosi::Yoko_line)
    {
        Serial.println("yoko");
    }
    else if (posi == LinePosi::Haji_line)
    {
        Serial.println("haji");
    }
    else
    {
        Serial.println("nai");
    }*/

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
    else // if (mode == Defender::Mode::BLUEGOAL)
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

    //// DEFENCE認識条件 ////
    bool defnece_ok =
        defence_goal_detected &&
        ((mode == Defender::Mode::YELLOWGOAL) ? (defence_goal_dis < DEFENCE_BLUE_GOAL_DIS) : (defence_goal_dis < DEFENCE_YELLOW_GOAL_DIS)) &&
        (defence_goal_deg > ((mode == Defender::Mode::YELLOWGOAL) ? DEFENCE_BLUE_GOAL_DEG_MAX : DEFENCE_YELLOW_GOAL_DEG_MAX) ||
         defence_goal_deg < ((mode == Defender::Mode::YELLOWGOAL) ? DEFENCE_BLUE_GOAL_DEG_MIN : DEFENCE_YELLOW_GOAL_DEG_MIN));

    //// ATTACK処理 ////
    now_ir_deg = irDeg(); // 今のirの角度更新

    bool is_ir_keep =
        // 厳しい条件分岐でアタッカーに移行する
        irDetected() &&
        abs(diffDeg(now_ir_deg, old_ir_deg)) <= 20 &&
        defnece_ok &&
        !(catchSensor.read() == HIGH);

    static bool come_back = false; // 帰還したか？

    if (is_ir_keep && come_back)
    {
        if (old_ir_keep_deg_flag == false) // 昔は一定ではなかったなら
        {
            ir_keep_deg_timer.reset();
            ir_keep_deg_flag = true;
        }
        old_ir_keep_deg_flag = true;

        if (ir_keep_deg_flag == true && ir_keep_deg_timer.msTime() >= 5000UL) // 5秒以上もボールが一定の角度にあるなら
        {
            attacking_timer.reset(); // アタッカータイマー開始
            attack_flag = true;
            come_back = false; // ボールを追いかけに行った
        }
    }
    else
    {
        old_ir_keep_deg_flag = false;
        ir_keep_deg_flag = false;
    }

    if (attack_flag == true &&
        (posi == LinePosi::Haji_line ||                              // アタックモードに移行して端ラインにいるなら戻る
         posi == LinePosi::Tate_line ||                              // アタックモードに移行して角ラインにいるなら戻る
         !irDetected() ||                                            // アタックモードに移行してボールがなくなったらら戻る
         (attacking_timer.msTime() > 1000UL && lineRingDetected()))) // アタックモードに移行して1秒経った後ラインが反応したら戻る
    {
        attack_flag = false;
    }

    if (attack_flag == true && attacking_timer.msTime() <= 3000UL) // 3ms間アタッカーをする
    {
        // まだ出ていないからcome_backをfalseにする
        if (defnece_ok)
        {
            come_back = false; // ボールを追いかけに行った
        }

        playAttacker((mode == Defender::Mode::YELLOWGOAL) ? Attacker::Mode::YELLOWGOAL : Attacker::Mode::BLUEGOAL);
        return;
    }
    else
    {
        attack_flag = false;
    }

    old_ir_deg = now_ir_deg; // 昔の角度記録

    //// DEFENCE処理 ////
    motorsPdProcess(&pd_gyro, bnoDeg(), 0); // PD更新制御

    if (defnece_ok)
    {
        if (lineRingDetected())
        {
            if (irDetected())
            {
                come_back = true; // 帰還した

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
                // ラインによる自己位置推定
                LinePosi posi = linePositionCheck();

                // ボールを守りたいベクトル
                Vector ir_defence_vec;
                if (posi == LinePosi::Haji_line) // 端だとこれ以上行かないようにまっすぐ進む(右：負、左：正で停止)
                {
                    if (defence_goal_deg > 0) // 左にある
                    {
                        if (defence_ir_deg < 0)
                        {
                            motorsMove(0, 0);
                        }
                        else
                        {
                            motorsMove(0, DEFENCE_HAJI_IR_FOLLOW_POWER_MAX);
                        }
                    }
                    else // 右にある
                    {
                        if (defence_ir_deg > 0)
                        {
                            motorsMove(0, 0);
                        }
                        else
                        {
                            motorsMove(0, DEFENCE_HAJI_IR_FOLLOW_POWER_MAX);
                        }
                    }
                }
                else if (posi == LinePosi::Yoko_line)
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
                else if (posi == LinePosi::Kado_line)
                {
                    int ir_near_sessen_deg;

                    if (defence_goal_deg > 0 && irDeg() >= 0 && irDeg() <= 140) // 右のラインにいてボールが左にある | (ゴール) (ボール) | (ロボット)
                    {
                        ir_near_sessen_deg = nearSeesenDeg(lineRingDeg(), defence_goal_deg - 90); // 接線方向へ行く
                    }
                    else if (defence_goal_deg <= 0 && irDeg() < 0 && irDeg() >= -140) // 左のラインにいてボールが右にある (ロボット) | (ボール) (ゴール) |
                    {
                        ir_near_sessen_deg = nearSeesenDeg(lineRingDeg(), defence_goal_deg + 90); // 接線方向へ行く
                    }
                    else
                    {
                        ir_near_sessen_deg = nearSeesenDeg(lineRingDeg(), defence_ir_deg); // 接線方向へ行く
                    }

                    ir_defence_vec = Vector(ir_near_sessen_deg, DEFENCE_KADO_IR_FOLLOW_POWER_MAX);
                }
                else // if (posi == LinePosi::Tate_line)
                {
                    if (defence_goal_deg > 0 && irDeg() >= 0 && irDeg() <= 140) // 右のラインにいてボールが左にある | (ゴール) (ボール) | (ロボット)
                    {
                        ir_defence_vec = Vector(0, DEFENCE_TATE_ZENSHIN_IR_FOLLOW_POWER_MAX);
                    }
                    else if (defence_goal_deg <= 0 && irDeg() < 0 && irDeg() >= -140) // 左のラインにいてボールが右にある (ロボット) | (ボール) (ゴール) |
                    {
                        ir_defence_vec = Vector(0, DEFENCE_TATE_ZENSHIN_IR_FOLLOW_POWER_MAX);
                    }
                    else
                    {
                        if (abs(irDeg()) < 90) // 前にある
                        {
                            ir_defence_vec = Vector(0, DEFENCE_TATE_ZENSHIN_IR_FOLLOW_POWER_MAX);
                        }
                        else // 後にある
                        {
                            ir_defence_vec = Vector(180, DEFENCE_TATE_KOUTAI_IR_FOLLOW_POWER_MAX);
                        }
                    }
                }

                float defence_ir_y = irDis() * sinf(radians(defence_ir_deg)); // y方向成分
                if (defence_ir_y >= DEFENCE_IR_FRONT_Y_MIN &&
                    defence_ir_y <= DEFENCE_IR_FRONT_Y_MAX) // 一定の値の中に入るとボールを追わなくなる (停止)
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
                // ゴールの中心へ行くベクトル
                Vector go_central_vec;
                LinePosi posi = linePositionCheck();

                if (posi == LinePosi::Yoko_line)
                {
                    if (defence_goal_deg > 0 && defence_goal_deg < 165)
                    {
                        go_central_vec = Vector(90, DEFENCE_GO_CENTRAL_POWER);
                    }
                    else if (defence_goal_deg <= 0 && defence_goal_deg <= -165)
                    {
                        go_central_vec = Vector(-90, DEFENCE_GO_CENTRAL_POWER);
                    }
                    else
                    {
                        go_central_vec = Vector(0, 0.0f);
                    }
                }
                else if (posi == LinePosi::Tate_line)
                {
                    go_central_vec = Vector(0, DEFENCE_GO_CENTRAL_POWER);
                }
                else // if(posi == LinePosi::Kado_line)
                {
                    int go_central_near_sessen_deg = nearSeesenDeg(lineRingDeg(), (defence_goal_deg > 0) ? defence_goal_deg - 90 : defence_goal_deg + 90);

                    go_central_vec = Vector(go_central_near_sessen_deg, DEFENCE_GO_CENTRAL_POWER);
                }

                // ライントレースするベクトル
                Vector line_trace_vec = Vector(lineRingDeg(), lineRingDis() * DEFENCE_LINE_TRACE_POWER_MAX / 100.0f);

                // 最終ベクトル
                Vector final_defence_vec = go_central_vec + line_trace_vec;

                motorsVectorMove(&final_defence_vec);
            }
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
            if (irDetected() && ((mode == Defender::Mode::YELLOWGOAL) ? (defence_goal_dis < DEFENCE_BLUE_GOAL_DIS) : (defence_goal_dis < DEFENCE_YELLOW_GOAL_DIS)))
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

            Vector final_modoru_vec = line_modoru_vec + ir_follow_vec; // 最終ベクトル

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