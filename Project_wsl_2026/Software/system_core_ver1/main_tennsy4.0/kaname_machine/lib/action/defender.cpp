#include "defender.hpp"

// PD制御
static PD pdGyro(0.4f, 1.6f); // ジャイロ用のPD調節値
static PD pdCam(0.9f, 1.1f);  // カメラ用のPD調節値

// 攻撃ゴールの位置
static bool attack_goal_detected;
static int attack_goal_deg;
static float attack_goal_dis;
// 守備ゴールの位置
static bool defence_goal_detected;
static int defence_goal_deg;
static float defence_goal_dis;

// 動き
static Vector LineTraceVec(PD *pd, int power);                               // ラインの中心に行こうとする（D成分は使わない）
static Vector LineTraceAndTargetVec(PD *line_trace_vec, int deg, int power); // ライントレースしながらdegが0のなる方向へに行く

void playDefender(Defender::Mode mode)
{
    motorsPdProcess(&pdGyro, bnoDeg(), 0);

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

    const int max_power = 90; // パワーの最大値

    const int defence_max_deg = 70;    // ディフェンスする最大角
    const int defence_limit_deg = 60;  // ボールの守備を継続する最大角
    const int defence_normal_deg = 30; // 対角線を使用せずディフェンスする最大角

    if (defence_goal_detected && defence_goal_dis < 100.0f) // 守備ゴールが見え、一定距離以内にゴールがあるなら
    {
        if (defence_goal_deg > 180 - defence_max_deg || defence_goal_deg < defence_max_deg - 180) // 後ろにゴールがあるなら(60~-60)（でフェンダーとしての動きにうつる）
        {
            if (lineRingDetected()) // ライン上にいるなら
            {
                PD pd(0.92f, 0.0f); // ライントレースのpd成分（d成分は使わない）
                Vector defence_vec;

                if (irDetected()) // ボールが見えるなら
                {
                    if (defence_goal_deg > 180 - defence_limit_deg || defence_goal_deg < defence_limit_deg - 180)
                    {
                        if (defence_goal_deg > 180 - defence_normal_deg || defence_goal_deg < defence_normal_deg - 180) // かなり後ろにあるなら(20~-20)
                        {
                            defence_vec = LineTraceAndTargetVec(&pd, irDeg(), max_power); // 守備する
                        }
                        else
                        {
                            int resetDeg = normalizeDeg(defence_goal_deg + 180);       // ゴールの対角方向の角度
                            int taikakuIrDeg = normalizeDeg(irDeg() - resetDeg + 360); // ゴールの対角方向の角度を基準とした時のIR角度

                            defence_vec = LineTraceAndTargetVec(&pd, taikakuIrDeg, max_power); // 対角線で守備する
                        }
                    }
                    else
                    {
                        int resetDeg = normalizeDeg(defence_goal_deg + 180);       // ゴールの対角方向の角度
                        int taikakuIrDeg = normalizeDeg(irDeg() - resetDeg + 360); // ゴールの対角方向の角度を基準とした時のIR角度

                        // Serial.print(taikakuIrDeg);

                        if (defence_goal_deg > 0 && taikakuIrDeg < 0) // リミットを超えたなら（右にいてボールはさらに右）
                        {
                            defence_vec = LineTraceVec(&pd, max_power); // ライントレースのみ
                        }
                        else if (defence_goal_deg < 0 && taikakuIrDeg > 0) // リミットを超えたなら（左にいてボールはさらに左）
                        {
                            defence_vec = LineTraceVec(&pd, max_power); // ライントレースのみ
                        }
                        else
                        {
                            defence_vec = LineTraceAndTargetVec(&pd, taikakuIrDeg, max_power); // 対角線で守備する
                        }
                    }
                }
                else // ボールが見えないなら
                {
                    defence_vec = LineTraceAndTargetVec(&pd, -(defence_goal_deg + 180), max_power); // 後ろを0度としてゴール中心に行こうとする
                }

                motorsVectorMove(&defence_vec); // 実行する
            }
            else
            {
                if (defence_goal_dis < 65.0f) // ゴールが近くにあるなら（ラインより前）
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
    pd->useD(false);                 // D成分は使わなくて十分
    pd->process(lineRingDis(), 0.0f); // 計算
    double len = map(fabs(pd->output()), 0.0f, 100.0f, 0.0f, power);

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
    if (abs(deg) < 6) // 目標角度がほぼ前
        sessen_len = 0.0f;
    else if (abs(deg) < 30)                                     // 目標角度が前付近
        sessen_len = map(abs(deg), 0.0f, 30.0f, 0.0f, behind_len); // 角度によってp制御
    else                                                        // 目標角度が前付近に無い
        sessen_len = behind_len;                                // マックスで動かす
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