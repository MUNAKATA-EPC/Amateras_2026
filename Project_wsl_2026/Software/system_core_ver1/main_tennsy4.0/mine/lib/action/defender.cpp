#include "defender.hpp"

// 攻撃ゴールの位置
static bool attack_goal_detected;
static int attack_goal_deg;
static float attack_goal_dis;
// 守備ゴールの位置
static bool defence_goal_detected;
static int defence_goal_deg;
static float defence_goal_dis;

// 昔のボールの角度記録
static int now_ir_deg = 0, old_ir_deg = 0;
static bool old_ir_keep_deg_flag = false;
static bool ir_keep_deg_flag = false, attack_flag = false;
static Timer ir_keep_deg_timer;
static Timer attacking_timer;

// 動き
static Vector LineTraceVec(PD *pd, int power);                               // ラインの中心に行こうとする（D成分は使わない）
static Vector LineTraceAndTargetVec(PD *line_trace_vec, int deg, int power); // ライントレースしながらdegが0のなる方向へに行く

static Timer line_timer;
static Timer kick_timer;
static bool old_catch = false;

static bool old_block_flag = false;

void playDefender(Defender::Mode mode)
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
    else // mode == Defender::Mode::BLUEGOAL
    {
        defence_goal_detected = yellowGoalDetected();
        defence_goal_deg = yellowGoalDeg();
        defence_goal_dis = yellowGoalDis();

        attack_goal_detected = blueGoalDetected();
        attack_goal_deg = blueGoalDeg();
        attack_goal_dis = blueGoalDis();
    }

    // キッカー
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
    // キッカー

    // アタッカーモードの移行する処理
    now_ir_deg = irDeg(); // 今のirの角度更新

    bool is_start_timer = irDetected() && irDis() <= 70 && abs(diffDeg(now_ir_deg, old_ir_deg)) <= 15 && (defence_goal_detected && defence_goal_dis <= 75) /*&& old_block_flag == true*/ && !(catchSensor.read() == HIGH); // 厳しい条件分岐でアタッカーに移行する

    if (is_start_timer)
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
            motorsMove(irDeg(), 40);
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

    // ディフェンスの処理
    motorsPdProcess(&pd_gyro, bnoDeg(), 0);

    old_block_flag = false; // falseにしておく

    const int max_power = 60; // パワーの最大値
    if (defence_goal_detected && defence_goal_dis <= 80)
    {
        if (lineRingDetected() && defence_goal_dis > 30)
        {
            if (irDetected())
            {
                // ライントレースのベクトル算出
                Vector line_trace_vec = Vector(lineRingDeg(), lineRingDis() * 0.3f); // 長さ最大：30

                // 対角線の角度算出
                int target_deg = normalizeDeg(defence_goal_deg + 180);
                int block_ir_deg = normalizeDeg(irDeg() - target_deg);
                if (defence_goal_deg >= 155 || defence_goal_deg <= -155) // （コート白線の横線の方にいる）なら普通に守る
                {
                    block_ir_deg = irDeg();
                }
                float block_ir_y = irDis() * sinf(radians(block_ir_deg)); // y方向成分

                // ブロックベクトルベクトル算出
                int block_deg;
                float block_len;

                if (defence_goal_deg >= 96 && defence_goal_deg <= -101 /*defence_goal_deg >= -105 && defence_goal_deg <= 109*/) // 前±110度にゴールが見えたなら（コート白線の後ろの端の方にいる）
                {
                    block_deg = fieldDeg();
                    block_len = max_power;
                }
                else
                {
                    bool is_right_detected = false, is_left_detected = false, is_front_detected = false, is_back_detected = false;

                    for (int i = 0; i < 16; i++)
                    {
                        if (lineSensorDetected(i))
                        {
                            if (i >= 1 && i <= 7) // 右のラインがある
                            {
                                is_right_detected = true;
                            }
                            if (i >= 9 && i <= 15) // 左のラインがある
                            {
                                is_left_detected = true;
                            }
                            if (i == 2 || i == 1 || i == 0 || i == 15 || i == 14) // 前にラインがある
                            {
                                is_front_detected = true;
                            }
                            if (i == 6 || i == 7 || i == 8 || i == 9 || i == 10) // 後にラインがある
                            {
                                is_back_detected = true;
                            }
                        }

                        if (is_right_detected && is_left_detected && is_front_detected && is_back_detected)
                        {
                            break;
                        }
                    }

                    if ((is_right_detected ^ is_left_detected) || (is_front_detected && is_back_detected)) // | (ゴール) | ←ここにいる可能性が高い
                    {
                        if (defence_goal_deg > 0 && irDeg() > 0 && irDeg() <= 140) // 右のラインにいてボールが左にある | (ゴール) (ボール) | (ロボット)
                        {
                            block_deg = 0;
                            block_len = max_power * 0.8f;
                        }
                        else if (defence_goal_deg <= 0 && irDeg() <= 0 && irDeg() >= -140) // 左のラインにいてボールが右にある (ロボット) | (ボール) (ゴール) |
                        {
                            block_deg = 0;
                            block_len = max_power * 0.8f;
                        }
                        else
                        {
                            if (irDeg() >= -90 && irDeg() <= 90)
                            {
                                block_deg = 0;
                                block_len = max_power * 0.5f;
                            }
                            else
                            {
                                block_deg = 180;
                                block_len = max_power * 0.5f;
                            }
                        }
                    }
                    else
                    {
                        if (block_ir_deg > 0) // 左にボールがある
                        {
                            if (false /*lineRingDis() >= 80.0f*/)
                            {
                                if (lineRingDeg() >= -90 && lineRingDeg() <= 90) // 後ろのロボットが行ってしまっているなら
                                {
                                    block_deg = 90 - 20;
                                    block_len = max_power * 0.88f;
                                }
                                else
                                {
                                    block_deg = 90 + 20;
                                    block_len = max_power * 0.88f;
                                }
                            }
                            else
                            {
                                block_deg = 90;
                                block_len = max_power * 0.88f;
                            }
                        }
                        else // 右にボールがある
                        {
                            if (lineRingDis() >= 80.0f)
                            {
                                if (lineRingDeg() >= -90 && lineRingDeg() <= 90) // 後ろのロボットが行ってしまっているなら
                                {
                                    block_deg = -90 + 20;
                                    block_len = max_power * 0.88f;
                                }
                                else
                                {
                                    block_deg = -90 - 20;
                                    block_len = max_power * 0.88f;
                                }
                            }
                            else
                            {
                                block_deg = -90;
                                block_len = max_power * 0.88f;
                            }
                        }
                    }

                    if (block_ir_deg >= 90 || block_ir_deg <= -90) // 後ろにボールがあるなら危険だからスピードを落とす
                    {
                        if (block_ir_y >= -10.0f && block_ir_y <= 10.0f)
                        {
                            block_len = max_power * 0.4f;
                        }
                    }
                }

                // 長さ補正（ゴールの反対方向を基準にして出力を調整し適切な場所で止まるようにする）
                if (block_ir_deg >= -90 && block_ir_deg <= 90) // 前方向にボールがあるなら
                {
                    if (block_ir_y >= -9.0f && block_ir_y <= 9.0f)
                    {

                        block_len = 0.0f;
                    }

                    if (block_ir_y >= -12.0f && block_ir_y <= 12.0f)
                    {
                        old_block_flag = true; // 記録
                    }
                }

                // ベクトル生成
                Vector block_vec = Vector(block_deg, block_len); // 長さ最大：100

                // 守備ベクトル算出
                Vector defence_vec = line_trace_vec + block_vec;

                if (defence_vec.length() >= max_power)
                {
                    for (float i = block_len; i >= 0.0f; i -= 1.0f)
                    {
                        // ベクトル生成
                        block_vec = Vector(block_deg, block_len); // 長さ最大：100

                        // 守備ベクトル算出
                        defence_vec = line_trace_vec + block_vec;

                        if (defence_vec.length() <= max_power)
                        {
                            break;
                        }
                    }
                }
                // 制御
                motorsVectorMove(&defence_vec);
            }
            else
            {
                // 後ろを0度としてゴール中心に行こうとするベクトル
                PD pd(0.3f, 0.0f); // ライントレースのpd成分（d成分は使わない）
                Vector goal_middle_vec = LineTraceAndTargetVec(&pd, -(defence_goal_deg + 180), max_power * 0.5f);
                // 制御
                motorsVectorMove(&goal_middle_vec);
            }
        }
        else
        {
            Vector modoru_vec;
            if (abs(defence_goal_deg) > 156)
            {
                if (defence_goal_dis <= 54.0f)
                {
                    modoru_vec = Vector(defence_goal_deg + 180, 30.0f);
                }
                else
                {
                    modoru_vec = Vector(defence_goal_deg, 30.0f);
                }
            }
            else
            {
                if (defence_goal_dis <= 57.0f)
                {
                    modoru_vec = Vector(defence_goal_deg + 180, 30.0f);
                }
                else
                {
                    modoru_vec = Vector(defence_goal_deg, 30.0f);
                }
            }

            if (irDetected())
            {
                int ir_follow_deg = (irDeg() > 0) ? 90 : -90;

                if (abs(diffDeg(modoru_vec.deg(), ir_follow_deg)) <= 175) // 合力が相殺されるのを防ぐ
                {
                    modoru_vec = modoru_vec + Vector(ir_follow_deg, 10.0f);

                    modoru_vec = Vector(modoru_vec.deg(), 40.0f); // 合力の力を50にする
                }
            }
            else
            {
                modoru_vec = modoru_vec * 1.0f;
            }

            motorsVectorMove(&modoru_vec);
        }
    }
    else if (lineRingDetected())
    {
        motorsMove(fieldDeg(), max_power);
    }
    else
    {
        // 定位置に戻る処理の角度計算
        int teiiti_deg;
        if (fieldDetected())
        {
            if (fieldDeg() >= 80 || fieldDeg() <= -80)
            {
                teiiti_deg = fieldDeg();
            }
            else
            {
                if (defence_goal_detected)
                {
                    teiiti_deg = defence_goal_deg;
                }
                else
                {
                    if (fieldDeg() > 0) // コート右にいる
                    {
                        teiiti_deg = 150;
                    }
                    else // コート左にいる
                    {
                        teiiti_deg = -150;
                    }
                }
            }
        }
        else
        {
            teiiti_deg = 180;
        }
        // IRボールがあったらオウンゴールになってしまうからそれを避ける
        if (irDetected())
        {
            if (abs(diffDeg(irDeg(), teiiti_deg)) <= 20 && irDis() <= 58) // 同じ方向にあり近い
            {
                if (irDeg() > 0) // 左にある
                {
                    motorsMove(teiiti_deg + 45, max_power * 0.8f);
                }
                else // 右にある
                {
                    motorsMove(teiiti_deg - 45, max_power * 0.8f);
                }
            }
            else
            {
                motorsMove(teiiti_deg, max_power * 0.8f);
            }
        }
        else
        {
            motorsMove(teiiti_deg, max_power * 0.8f);
        }
    }
}

Vector LineTraceVec(PD *pd, int power)
{
    pd->useD(false);                         // D成分は使わなくて十分
    pd->process(lineRingDis(), 0.0f, false); // 計算
    float len = fabs(pd->output()) * power / 100.0f;

    return getVec(lineRingDeg(), len);
}

Vector LineTraceAndTargetVec(PD *pd_line_trace, int deg, int power)
{
    // -180~180に変換する
    deg = normalizeDeg(deg);

    // ライントレースベクトルの算出
    Vector line_trace_vec = LineTraceVec(pd_line_trace, power);

    float behind_len = sqrtf(power * power - line_trace_vec.length() * line_trace_vec.length()); // 残りのベクトルの長さ

    // 接線ベクトル角度の算出（lineRingDeg()の接線方向の内degに近いほう）
    int round_line_ring_deg = areaIndexFromDeg(lineRingDeg(), 8) * 360 / 8;
    int sessen_deg = nearSeesenDeg(round_line_ring_deg, deg);
    // 接線ベクトル長さの算出
    float sessen_len;
    if (abs(deg) < 10) // 目標角度がほぼ前
    {
        sessen_len = 0.0f;
    }
    else if (abs(deg) < 16) // 目標角度が前付近
    {
        sessen_len = abs(deg) * behind_len / 16.0f; // 角度によってp制御
    }
    else // 目標角度が前付近に無い
    {
        sessen_len = behind_len; // マックスで動かす
    }
    // 接線ベクトル算出
    Vector sessen_vec(sessen_deg, sessen_len);

    // スケーリング（安全対策）
    Vector vec = line_trace_vec + sessen_vec;
    if (vec.length() > power)
    {
        float scale = power / vec.length();
        vec = vec * scale;
    }

    return vec;
}