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

#else

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

// 直線y=ax+bと円r^2=x^2+y^2の交点算出関数
#include <cmath> // std::sqrt, std::NAN, std::fabs
int lineAndCircleCrossPoint(float a_of_line, float b_of_line, float r_of_circle, float *cross_point1_ptr, float *cross_point2_ptr)
{
    float a = a_of_line;
    float b = b_of_line;
    float r = r_of_circle;

    // ルートの中身 (判別式の D/4): r^2 * (1 + a^2) - b^2
    float roots_content = r * r * (1.0f + a * a) - b * b;
    const float EPSILON = 1e-6f; // 浮動小数点比較の許容誤差

    if (roots_content < -EPSILON)
    {
        cross_point1_ptr[0] = cross_point1_ptr[1] = 0.0f;
        cross_point2_ptr[0] = cross_point2_ptr[1] = 0.0f;
        return 0; // 交点なし
    }

    float sqrt_roots_content = std::sqrt(roots_content); // ルート部分の計算
    float denominator_content = 1.0f + a * a;            // 分母部分の計算 (1 + a^2)

    // x座標の計算 (二次方程式の解の公式)
    // x = (-ab ± sqrt(D/4)) / (1 + a^2)

    // 交点1 (x1)
    cross_point1_ptr[0] = (-a * b + sqrt_roots_content) / denominator_content;
    // 交点2 (x2)
    cross_point2_ptr[0] = (-a * b - sqrt_roots_content) / denominator_content;

    // y座標の計算

    // y1 = a * x1 + b
    cross_point1_ptr[1] = a * cross_point1_ptr[0] + b;
    // y2 = a * x2 + b
    cross_point2_ptr[1] = a * cross_point2_ptr[0] + b;

    if (std::fabs(roots_content) <= EPSILON)
    {
        return 1; // わずかに接する場合
    }
    return 2; // 二点の交点を持つ場合
}

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

    // アタッカーモードの移行する処理
    now_ir_deg = irDeg(); // 今のirの角度更新

    bool is_start_timer = irDetected() && abs(diffDeg(now_ir_deg, old_ir_deg)) <= 15 && irDis() <= 60 && (defence_goal_detected && defence_goal_dis <= 80 && (defence_goal_deg > 120 || defence_goal_deg < -120)); // 厳しい条件分岐でアタッカーに移行する

    if (is_start_timer)
    {
        if (old_ir_keep_deg_flag == false) // 昔は一定ではなかったなら
        {
            ir_keep_deg_timer.reset();
            ir_keep_deg_flag = true;
        }
        old_ir_keep_deg_flag = true;

        if (ir_keep_deg_flag == true && ir_keep_deg_timer.msTime() >= 4500UL) // 4.5秒以上もボールが一定の角度にあるなら
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

    if (attack_flag == true && !irDetected())
    {
        attack_flag = false;
    }

    if (attack_flag == true && attacking_timer.msTime() <= 3000UL) // 3ms間アタッカーをする
    {
        playAttacker(Attacker::Mode::GYRO);
        return;
    }
    else
    {
        attack_flag = false;
    }

    old_ir_deg = now_ir_deg; // 昔の角度記録

    // ディフェンスの処理
    motorsPdProcess(&pd_gyro, bnoDeg(), 0);

    const int max_power = 95; // パワーの最大値
    if (defence_goal_detected && defence_goal_dis <= 80)
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
                float block_ir_y = irDis() * sinf(radians(block_ir_deg)); // y方向成分
                if (defence_goal_deg >= 155 || defence_goal_deg <= -155)  // （コート白線の横線の方にいる）なら普通に守る
                {
                    block_ir_deg = irDeg();
                }

                // ブロックベクトルベクトル算出
                int block_deg;
                float block_len;

                if (defence_goal_deg >= -116 && defence_goal_deg <= 103) // 前±110度にゴールが見えたなら（コート白線の後ろの端の方にいる）
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
                            i = 16; // for文を抜ける
                        }
                    }

                    if ((is_right_detected ^ is_left_detected) || (is_front_detected && is_back_detected)) // | (ゴール) | ←ここにいる可能性が高い
                    {
                        if (irDis() > 51)
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
                            if (lineRingDis() >= 10.0f)
                            {
                                if (lineRingDeg() >= -90 && lineRingDeg() <= 90) // 後ろのロボットが行ってしまっているなら
                                {
                                    block_deg = 90 - 20;
                                    block_len = max_power;
                                }
                                else
                                {
                                    block_deg = 90 + 20;
                                    block_len = max_power;
                                }
                            }
                            else
                            {
                                block_deg = 90;
                                block_len = max_power;
                            }
                        }
                        else // 右にボールがある
                        {
                            if (lineRingDis() >= 10.0f)
                            {
                                if (lineRingDeg() >= -90 && lineRingDeg() <= 90) // 後ろのロボットが行ってしまっているなら
                                {
                                    block_deg = -90 + 20;
                                    block_len = max_power;
                                }
                                else
                                {
                                    block_deg = -90 - 20;
                                    block_len = max_power;
                                }
                            }
                            else
                            {
                                block_deg = -90;
                                block_len = max_power;
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
                    if (block_ir_y >= -6.0f && block_ir_y <= 6.0f)
                    {
                        block_len = 0.0f;
                    }
                    /*
                    else if (block_ir_y >= -5.0f && block_ir_y <= 5.0f)
                    {
                        // 1 = 17.5 * a + b
                        // 0 = 10 * a + b
                        // 1 / 7.5 = a,b = 1 - 17.5 * 1 / 7.5
                        block_len = block_len * ((fabs(block_ir_y) - 6.0f) / 5.0f);
                    }
                    */
                }

                // ベクトル生成
                Vector block_vec = Vector(block_deg, block_len); // 長さ最大：100

                // 守備ベクトル算出
                Vector defence_vec = line_trace_vec + block_vec;

                // defence_vec の長さが max_power を超えた場合の制限処理
                if (defence_vec.length() >= max_power)
                {
                    // 直線の定義 (y = ax + b)
                    float line_gain_a; // 傾き a
                    float line_gain_b; // 切片 b
                    float block_deg_rad = block_vec.rad();

                    // 垂直線の境界処理: 89.5度から90.5度を垂直と見なす
                    bool is_nearly_vertical = (abs(block_deg_rad) > radians(88.0f) && abs(block_deg_rad) < radians(92.0f));

                    if (is_nearly_vertical)
                    {
                        line_gain_a = tanf(radians(block_vec.deg() + 5.0f)); // わずかに傾け、(0.0f,0.0f)を回避

                        // 切片 b = yA - a * xA
                        line_gain_b = line_trace_vec.y() - line_gain_a * line_trace_vec.x();
                    }
                    else
                    {
                        // 通常の線 (y = ax + b) の場合
                        // 傾き a は block_vec の角度から求める
                        line_gain_a = tanf(block_deg_rad);

                        // 切片 b = yA - a * xA (yA, xA は line_trace_vec の終点座標)
                        line_gain_b = line_trace_vec.y() - line_gain_a * line_trace_vec.x();
                    }

                    // 交点計算
                    float cross_point_of_max_power_ring1[2]; // 交点1 {x1, y1}
                    float cross_point_of_max_power_ring2[2]; // 交点2 {x2, y2}

                    int cross_point_count = lineAndCircleCrossPoint(
                        line_gain_a,
                        line_gain_b,
                        (float)max_power,
                        cross_point_of_max_power_ring1,
                        cross_point_of_max_power_ring2); // 交点計算

                    // 最終ベクトル (defence_vec) の決定
                    if (cross_point_count == 0) // 交点が0個 (ありえないはずだが安全策)
                    {
                        // ライントレースのみをする
                        defence_vec = line_trace_vec;
                    }
                    else if (cross_point_count == 1) // 交点が1個 (接する場合)
                    {
                        // 交点座標がそのまま制限された合力の終点となる
                        defence_vec = Vector(0.0f, 0.0f, cross_point_of_max_power_ring1[0], cross_point_of_max_power_ring1[1]);
                    }
                    else // 交点が2個 (最も元の合力に近い交点を選ぶ)
                    {
                        // 元の合力 R の終点 (defence_vec.x, defence_vec.y)
                        float rx = defence_vec.x();
                        float ry = defence_vec.y();

                        // 交点1 (P_R1) と元のRの距離の二乗
                        float dist1_sq = powf(rx - cross_point_of_max_power_ring1[0], 2) +
                                         powf(ry - cross_point_of_max_power_ring1[1], 2);

                        // 交点2 (P_R2) と元のRの距離の二乗
                        float dist2_sq = powf(rx - cross_point_of_max_power_ring2[0], 2) +
                                         powf(ry - cross_point_of_max_power_ring2[1], 2);

                        if (dist1_sq < dist2_sq)
                        {
                            // 交点1が元のベクトルRに最も近い
                            defence_vec = Vector(0.0f, 0.0f, cross_point_of_max_power_ring1[0], cross_point_of_max_power_ring1[1]);
                        }
                        else
                        {
                            // 交点2が元のベクトルRに最も近い
                            defence_vec = Vector(0.0f, 0.0f, cross_point_of_max_power_ring2[0], cross_point_of_max_power_ring2[1]);
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
            if (defence_goal_deg >= 155 || defence_goal_deg <= -155) // 真後ろにある場合
            {
                if (defence_goal_dis <= 67.0f)
                {
                    motorsMove(defence_goal_deg + 180, max_power * 0.5f);
                }
                else
                {
                    motorsMove(defence_goal_deg, max_power * 0.5f);
                }
            }
            else // 横にある場合
            {
                if (defence_goal_dis <= 69.0f)
                {
                    motorsMove(defence_goal_deg + 180, max_power * 0.5f);
                }
                else
                {
                    motorsMove(defence_goal_deg, max_power * 0.5f);
                }
            }
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
                    motorsMove(irDeg() + 60, max_power * 0.8f);
                }
                else // 右にある
                {
                    motorsMove(irDeg() - 60, max_power * 0.8f);
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

#endif