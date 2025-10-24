#include "defender.hpp"

// defenceで使う行動パターン
static void Defence(int power, bool useTaikaku);
static void TeiitiModoru(int power, int keepDis);

static AnglePD pdGyro(0.8, 0.1); // ジャイロ用のPD調節値
static AnglePD pdCam(0.6, 0.1);  // カメラ用のPD調節値

void playDefender()
{
    motorsPdProcess(&pdGyro, bnoDeg(), 0);

    if (yellowGoalDetected()) // 黄色ゴール見えてて
    {
        if (yellowGoalDis() < 85) // 近い距離にあって
        {
            const int backDeg = 60;

            if (yellowGoalDeg() > 180 - backDeg && yellowGoalDeg() < 180 + backDeg) // 後ろにゴールあって
            {
                if (lineRingDetected()) // ラインが反応していてるならディフェンスの動き
                {
                    const int TaikakuBackDeg = 35; // 対角線で守備する角度

                    if (yellowGoalDeg() > 180 - TaikakuBackDeg && yellowGoalDeg() < 180 + TaikakuBackDeg) // 後ろにゴールあって
                    {
                        Defence(90, false);
                    }
                    else
                    {
                        Defence(90, true);
                    }
                }
                else // ラインが反応していていないならゴールへ行く動き
                {
                    Serial.print(yellowGoalDeg());
                    Serial.print(" ");
                    Serial.println(yellowGoalDis());

                    if (yellowGoalDis() < 60)
                    {
                        motorsMove(yellowGoalDeg() + 180, 60);
                    }
                    else
                    {
                        motorsMove(yellowGoalDeg(), 60);
                    }
                }
            }
            else // ゴールが後ろに無い
            {
                if (lineRingDetected()) // ラインが反応していてるならラインから逃れる動き
                {
                    motorsMove(lineRingDeg() + 180, 60);
                }
                else // ラインが反応していていないならゴールから一定の距離を保ちながら定位置へ移動
                {
                    TeiitiModoru(90, 80);
                }
            }
        }
        else
        {
            TeiitiModoru(60, 80);
        }
    }
    else if (lineRingDetected())
    {
        if (area4(lineRingDeg()) == Area4::BACK)
        {
            motorsMove(fieldDeg(), 70);
        }
        else
        {
            motorsMove(lineRingDeg() + 180, 70);
        }
    }
    else
    {
        // 円形ラインで反応なし
        Serial.println("none");

        if (areaIndexFromDeg(2, fieldDeg()) == 1)
        {
            motorsMove(fieldDeg(), 75);
        }
        else
        {
            motorsMove(180, 70);
        }
    }
}

// ディフェンス
static PD pdLineTrace(0.56, 0.0); // ライントレース用のPD調節値

static void Defence(int power, bool useTaikaku)
{
    int resetDeg = (yellowGoalDetected() && useTaikaku) ? (yellowGoalDeg() + 180) % 360 : 0; // ゴールの対角方向の角度
    int taikakuIrDeg = (irDeg() - resetDeg + 360) % 360;                                     // ゴールの対角方向の角度を基準とした時のIR角度

    /* --詳しい算出方法は物xを見るべし-- */
    double maxLen = (double)power; // ディフェンス時の最大長さ
    const int frontDeg = 50;       // 前の長さ（この角度を区切りにロボットは停止する制御に移る）

    pdLineTrace.process(lineRingDis(), 0.0); // ライントレース用PD計算

    // ライトレース用ベクトル
    double lineLen = constrain(fabs((double)pdLineTrace.output()), 0.0, maxLen);
    Vector lineTraceVec(lineRingDeg(), lineLen);

    // 対角方向を基準としたIRボールの方向の接線方向の算出
    int lineRingRoundDeg = areaIndexFromDeg(8, lineRingDeg()) * 360 / 8; // 精度向上のため粗目の角度を計算（矛盾）
    int irSessenDeg = nearSeesenDeg(lineRingRoundDeg, taikakuIrDeg);     // どっちの接線がIRボールの角度に近いか
    // 対角方向を基準としたIRボールの方向用ベクトルの長さの最大値（ラインの角度の接線方向）（ライトレースの残りのベクトルで生成）
    double irMaxLen = sqrt(maxLen * maxLen - lineTraceVec.length() * lineTraceVec.length());
    // 対角方向を基準としたIRボールの角度によってベクトルの長さを変える
    double irDegVal = (taikakuIrDeg < 180) ? (double)taikakuIrDeg : 360.0 - (double)taikakuIrDeg;
    double irLen = (taikakuIrDeg < frontDeg || taikakuIrDeg > 360 - frontDeg) ? fabs(irDegVal * irMaxLen / (double)frontDeg) : irMaxLen;
    irLen = constrain(irLen, 0.0, irMaxLen);
    // ベクトル生成
    Vector irSessenVec(irSessenDeg, irLen);

    // 移動ベクトルの合成
    Vector moveVec = irDetected() ? lineTraceVec + irSessenVec : lineTraceVec;
    if (moveVec.length() > maxLen)
    {
        moveVec = moveVec * (maxLen / moveVec.length());
    }

    motorsVectorMove(&moveVec);

    Serial.print("lineTraceVec(" + String(lineTraceVec.deg()) + "," + String(lineTraceVec.length()) + ")");
    Serial.print(" + ");
    Serial.print("irSessenVec(" + String(irSessenVec.deg()) + "," + String(irSessenVec.length()) + ")");
    Serial.print(" = ");
    Serial.println("moveVec(" + String(moveVec.deg()) + "," + String(moveVec.length()) + ")");
}

// ゴールと距離保ちながら戻る
static PD pdKeepDisFromGoal(0.6, 0); // ゴールと一定の距離を保ちながら定位置へ移動用のPD調整値

static void TeiitiModoru(int power, int keepDis)
{
    double maxLen = (double)power; // ディフェンス時の最大長さ

    pdKeepDisFromGoal.process(yellowGoalDis(), keepDis); // pd計算

    int keepDisDeg = yellowGoalDis() < keepDis ? yellowGoalDeg() : yellowGoalDeg() + 180; // ゴールからの距離によって移動方向をかえる
    double keepDisLen = fabs(pdKeepDisFromGoal.output());
    keepDisLen = constrain(keepDisLen, 0.0, maxLen);
    Vector keepDisVec(keepDisDeg, keepDisLen);

    int goalSessenDeg = nearSeesenDeg(yellowGoalDeg(), (yellowGoalDeg() < 180) ? 90 : 270);
    double goalSessenLen = sqrt(maxLen * maxLen - keepDisVec.length() * keepDisVec.length());
    goalSessenLen = constrain(goalSessenLen, 0.0, maxLen);
    Vector goalSessenVec(goalSessenDeg, goalSessenLen);

    Vector moveVec = keepDisVec + goalSessenVec;
    if (moveVec.length() > maxLen)
    {
        moveVec = moveVec * (maxLen / moveVec.length());
    }

    motorsVectorMove(&moveVec);
}