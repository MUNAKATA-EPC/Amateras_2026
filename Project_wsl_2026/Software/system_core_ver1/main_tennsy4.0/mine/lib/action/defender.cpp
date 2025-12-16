#include "defender.hpp"

// #define OLD

#ifdef OLD

// 攻撃ゴールの位置
static bool attack_goal_detected;
static int attack_goal_deg;
static float attack_goal_dis;
// 守備ゴールの位置
static bool defence_goal_detected;
static int defence_goal_deg;
static float defence_goal_dis;

// 動き
static Vector LineTraceVec(PD *pd, int power);                                          // ラインの中心に行こうとする（D成分は使わない）
static Vector LineTraceAndTargetVec(PD *line_trace_vec, int deg, float dis, int power); // ライントレースしながらdegが0のなる方向へに行く

// 昔のボールの角度記録
static int now_ir_deg = 0, old_ir_deg = 0;
static Timer ir_keep_deg_timer;
static Timer attacking_timer;

void playDefender(Defender::Mode mode)
{
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

    now_ir_deg = irDeg(); // 今のirの角度更新
    if (!irDetected() || abs(diffDeg(now_ir_deg, old_ir_deg)) > 15 || (defence_goal_detected && defence_goal_dis <= 85.0f))
        ir_keep_deg_timer.reset();

    if (attacking_timer.everReset() && attacking_timer.msTime() < 5000)
    {
        playAttacker(Attacker::Mode::GYRO /*(mode == Defender::YELLOWGOAL) ? Attacker::YELLOWGOAL : Attacker::BLUEGOAL*/);
        return;
    }
    else if (ir_keep_deg_timer.everReset() && ir_keep_deg_timer.msTime() > 9000)
    {
        playAttacker(Attacker::Mode::GYRO /*(mode == Defender::YELLOWGOAL) ? Attacker::YELLOWGOAL : Attacker::BLUEGOAL*/);
        attacking_timer.reset();
        return;
    }
    old_ir_deg = now_ir_deg; // 昔の角度記録

    motorsPdProcess(&pd_gyro, bnoDeg(), 0);

    const int max_power = 80; // パワーの最大値

    const int defence_max_deg = 70;    // ディフェンスする最大角
    const int defence_limit_deg = 60;  // ボールの守備を継続する最大角
    const int defence_normal_deg = 30; // 対角線を使用せずディフェンスする最大角

    if (defence_goal_detected && defence_goal_dis <= 85.0f) // 守備ゴールが見え、一定距離以内にゴールがあるなら
    {
        if (abs(diffDeg(defence_goal_deg, 180)) < defence_max_deg) // 後ろにゴールがあるなら(60~-60)（でフェンダーとしての動きにうつる）
        {
            if (lineRingDetected()) // ライン上にいるなら
            {
                PD pd(0.11f, 0.0f); // ライントレースのpd成分（d成分は使わない）
                Vector defence_vec;

                if (irDetected()) // ボールが見えるなら
                {
                    if (abs(diffDeg(defence_goal_deg, 180)) < defence_limit_deg)
                    {
                        if (abs(diffDeg(defence_goal_deg, 180)) < defence_normal_deg) // かなり後ろにあるなら
                        {
                            defence_vec = LineTraceAndTargetVec(&pd, now_ir_deg, irDis(), max_power); // 守備する
                        }
                        else
                        {
                            int resetDeg = normalizeDeg(defence_goal_deg + 180);          // ゴールの対角方向の角度
                            int taikakuIrDeg = normalizeDeg(now_ir_deg - resetDeg + 360); // ゴールの対角方向の角度を基準とした時のIR角度

                            defence_vec = LineTraceAndTargetVec(&pd, taikakuIrDeg, irDis(), max_power); // 対角線で守備する
                        }
                    }
                    else
                    {
                        int resetDeg = normalizeDeg(defence_goal_deg + 180);          // ゴールの対角方向の角度
                        int taikakuIrDeg = normalizeDeg(now_ir_deg - resetDeg + 360); // ゴールの対角方向の角度を基準とした時のIR角度

                        if (defence_goal_deg > 0 && taikakuIrDeg < 0) // リミットを超えたなら（左に傾いていてボールはさらに右）
                        {
                            defence_vec = LineTraceVec(&pd, max_power * 0.6f); // ライントレースのみ
                        }
                        else if (defence_goal_deg < 0 && taikakuIrDeg > 0) // リミットを超えたなら（右に傾いていてボールはさらに左）
                        {
                            defence_vec = LineTraceVec(&pd, max_power * 0.6f); // ライントレースのみ
                        }
                        else
                        {
                            defence_vec = LineTraceAndTargetVec(&pd, taikakuIrDeg, irDis(), max_power); // 対角線で守備する
                        }
                    }
                }
                else // ボールが見えないなら
                {
                    defence_vec = LineTraceAndTargetVec(&pd, -(defence_goal_deg + 180), irDis(), max_power); // 後ろを0度としてゴール中心に行こうとする
                }

                motorsVectorMove(&defence_vec); // 実行する
            }
            else
            {
                if (defence_goal_dis <= 72.0f) // ゴールが近くにあるなら（ラインより前）
                {
                    motorsMove(defence_goal_deg + 180, max_power); // ゴールから離れる
                }
                else // ゴールが遠くにあるなら
                {
                    motorsMove(defence_goal_deg, max_power); // ゴールに近寄る
                }
            }
        }
        else // 後ろにゴールがないなら
        {
            motorsMove(fieldDeg(), max_power); // コートの中心へ行く
        }
    }
    else if (lineRingDetected()) // （ゴールが見えていないときに）ラインが反応したなら
    {
        motorsMove(fieldDeg(), max_power); // コートの中心へ行く
    }
    else
    {
        if (defence_goal_detected) // 守備ゴールが見えるなら
        {
            motorsMove(defence_goal_deg, max_power);
        }
        else // 守備ゴールが見えないなら
        {
            if (abs(fieldDeg()) > 90) // 前にコートの中心があるなら
            {
                motorsMove(180, max_power); // 後ろへ行く
            }
            else // 後ろにコートの中心があるなら
            {
                if (fieldDeg() < 0) // 右にコートの中心があるなら
                {
                    motorsMove(-135, max_power); // 右後ろへ行く
                }
                else
                {
                    motorsMove(135, max_power); // 左後ろへ行く
                }
            }
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

Vector LineTraceAndTargetVec(PD *pd_line_trace, int deg, float dis, int power)
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
    if (dis <= 54.0f)
    {
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
    }
    else
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

#endif

#ifndef OLD

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
static Timer ir_keep_deg_timer;
static Timer attacking_timer;

// 動き
static Vector LineTraceVec(PD *pd, int power);                               // ラインの中心に行こうとする（D成分は使わない）
static Vector LineTraceAndTargetVec(PD *line_trace_vec, int deg, int power); // ライントレースしながらdegが0のなる方向へに行く

void playDefender(Defender::Mode mode)
{
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

    now_ir_deg = irDeg(); // 今のirの角度更新
    if (!irDetected() || abs(diffDeg(now_ir_deg, old_ir_deg)) > 15 || (defence_goal_detected && defence_goal_dis <= 85.0f))
        ir_keep_deg_timer.reset();

    if (attacking_timer.everReset() && attacking_timer.msTime() < 5000)
    {
        playAttacker(Attacker::Mode::GYRO /*(mode == Defender::YELLOWGOAL) ? Attacker::YELLOWGOAL : Attacker::BLUEGOAL*/);
        return;
    }
    else if (ir_keep_deg_timer.everReset() && ir_keep_deg_timer.msTime() > 9000)
    {
        playAttacker(Attacker::Mode::GYRO /*(mode == Defender::YELLOWGOAL) ? Attacker::YELLOWGOAL : Attacker::BLUEGOAL*/);
        attacking_timer.reset();
        return;
    }
    old_ir_deg = now_ir_deg; // 昔の角度記録

    motorsPdProcess(&pd_gyro, bnoDeg(), 0);

    const int max_power = 95; // パワーの最大値

    fullColorLed1.rgbLightUp(0, 0, 0);

    if (defence_goal_detected)
    {
        if (lineRingDetected())
        {
            if (irDetected())
            {
                // ライントレースのベクトル算出
                Vector line_trace_vec = Vector(lineRingDeg(), float(lineRingDis() * lineRingDis() / 100.0f * 0.5f)); // 長さ最大：100

                // 対角線の角度算出
                int target_deg = normalizeDeg(defence_goal_deg + 180);
                int block_ir_deg = normalizeDeg(irDeg() - target_deg);
                if (defence_goal_deg >= 160 || defence_goal_deg <= -160) // 後ろにゴールが有ったら普通で守る
                {
                    block_ir_deg = irDeg();
                }

                // ブロックベクトルベクトル算出
                int block_deg;
                float block_len;

                if (abs(defence_goal_deg) < 120) // 前±120度にゴールが見えたなら
                {
                    Serial.println("1");
                    block_deg = fieldDeg();
                    block_len = max_power;
                }
                else
                {
                    if (abs(defence_goal_deg) < 150) // 前±150度にゴールが見えたなら
                    {
                        Serial.println("2");
                        if (irDis() < 57) // 近くにボールがある
                        {
                            if (abs(block_ir_deg) < 90) // 前にボールがある
                            {
                                block_deg = 0;
                                block_len = max_power * 0.7f;
                            }
                            else // 後にボールがある
                            {
                                block_deg = 180;
                                block_len = max_power * 0.4f;
                            }
                        }
                        else
                        {
                            block_deg = 0;
                            block_len = max_power * 0.5f;
                        }
                    }
                    else
                    {
                        Serial.println("3");
                        if (block_ir_deg > 0) // 左にボールがある
                        {
                            block_deg = 90;
                            block_len = max_power;
                        }
                        else // 右にボールがある
                        {
                            block_deg = -90;
                            block_len = max_power;
                        }
                    }
                }
                // 長さ補正（ゴールの反対方向を基準にして出力を調整する）
                if (block_ir_deg >= -10 && block_ir_deg <= 10)
                {
                    block_len = 0.0f;
                }
                else if (block_ir_deg >= -20 && block_ir_deg <= 20)
                {
                    // 1 = 20 * a + b
                    // 0 = 10 * a + b
                    // 1 / 10 = a,b = -1
                    block_len = block_len * (0.1f * abs(block_ir_deg) - 1.0f);
                }
                else if (block_ir_deg <= -135 || block_ir_deg >= 135)
                {
                    block_len = block_len * 0.5f;
                }
                // ベクトル生成
                Vector block_vec = Vector(block_deg, block_len); // 長さ最大：100

                // 守備ベクトル算出
                Vector defence_vec = line_trace_vec + block_vec;
                if (defence_vec.length() >= max_power)
                {
                    // 角度の差
                    float diff_rad = radians(abs(diffDeg(line_trace_vec.deg(), block_vec.deg())));
                    // line_trace_vec^2 * sin^2(φ) の項
                    float term_sin = line_trace_vec.length() * line_trace_vec.length() * sinf(diff_rad) * sinf(diff_rad);
                    // defence_vec^2 の項
                    float term_R = max_power * max_power;
                    // ルートの中身
                    float root_content = term_R - term_sin;
                    // line_trace_vec * cos(φ) の項
                    float term_cos = line_trace_vec.length() * cosf(diff_rad);
                    // final_block_lenの算出
                    float final_block_len = sqrtf(root_content) - term_cos;
                    // 書き換え
                    defence_vec = line_trace_vec + Vector(block_deg, final_block_len);
                }

                // 制御
                motorsVectorMove(&defence_vec);
            }
            else
            {
                // 後ろを0度としてゴール中心に行こうとするベクトル
                PD pd(0.5f, 0.0f); // ライントレースのpd成分（d成分は使わない）
                Vector goal_middle_vec = LineTraceAndTargetVec(&pd, -(defence_goal_deg + 180), max_power);
                // 制御
                motorsVectorMove(&goal_middle_vec);
            }
        }
        else
        {
            if (defence_goal_dis <= 80.0f)
            {
                motorsMove(defence_goal_deg + 180, max_power * 0.6f);
            }
            else
            {
                motorsMove(defence_goal_deg, max_power * 0.6f);
            }
        }
    }
    else if (lineRingDetected())
    {
        motorsMove(fieldDeg(), max_power);
    }
    else
    {
        // 定位置に戻る処理
        if (fieldDetected())
        {
            if (fieldDeg() > 90 || fieldDeg() < -90)
            {
                motorsMove(fieldDeg(), max_power * 0.5f);
            }
            else
            {
                if (defence_goal_detected)
                {
                    motorsMove(defence_goal_deg, max_power * 0.5f);
                }
                else
                {
                    if (fieldDeg() > 0) // コート右にいる
                    {
                        motorsMove(120, max_power * 0.5f);
                    }
                    else // コート左にいる
                    {
                        motorsMove(-120, max_power * 0.5f);
                    }
                }
            }
        }
        else
        {
            motorsMove(180, max_power * 0.5f);
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

#endif