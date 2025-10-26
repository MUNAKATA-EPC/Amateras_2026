#include "defender.hpp"

// defenceで使う行動パターン
static Vector Defence(int power, bool useTaikaku, bool backToLineMiddle);
static Vector TeiitiModoru(int power, int keepDis);
static Vector LineEscape(int power);

static AnglePD pdGyro(0.8, 0.1); // ジャイロ用のPD調節値
static AnglePD pdCam(0.6, 0.1);  // カメラ用のPD調節値

void playDefender(Defender::Mode mode)
{
    Vector defenderVec;              // 最終的な移動方向・力格納
    bool lineEscapeMovement = false; // これがtrueならIRの位置関係なくラインから逃れる

    motorsPdProcess(&pdGyro, bnoDeg(), 0);

    Serial.println(yellowGoalDeg());

    if (yellowGoalDetected()) // 黄色ゴール見えてて
    {
        // 後ろを0度として範囲指定
        const int backDeg = 40;         // 守備範囲
        const int defenceLimitDeg = 35; // ボールを追おうとするが守れる範囲のギリギリだったらそこで停止する
        const int TaikakuBackDeg = 22;  // 対角線で”守備しない”角度

        if (abs(yellowGoalDeg() - 180) < backDeg) // 後ろにゴールあって
        {
            if (lineRingDetected())
            {
                lineEscapeMovement = true;

                bool irDefence = (irDetected() && irDis() < 160) ? true : false; // irボールがありかつ近くなら

                if (abs(yellowGoalDeg() - 180) < TaikakuBackDeg) // かなり後ろにゴールがあって
                {
                    defenderVec = Defence(90, false, irDefence);
                }
                else
                {
                    defenderVec = Defence(90, true, irDefence);
                }

                if (abs(yellowGoalDeg() - 180) > defenceLimitDeg) // リミットを超えたら停止
                {
                    defenderVec = getVec(0, 0.0);
                }
            }
            else
            {
                if (yellowGoalDis() < 82)
                {
                    defenderVec = getVec(yellowGoalDeg() + 180, 60);
                }
                else
                {
                    defenderVec = getVec(yellowGoalDeg(), 60);
                }
            }
        }
        else
        {
            if (lineRingDetected())
            {
                lineEscapeMovement = true;

                defenderVec = getVec(fieldDeg(), 90);
            }
            else
            {
                defenderVec = TeiitiModoru(60, 90);
            }
        }
    }
    else if (lineRingDetected()) // ラインが反応していてるならラインから逃れる動き
    {
        lineEscapeMovement = true;

        // LineEscape(90);
        defenderVec = getVec(fieldDeg(), 60);
    }
    else // 円形ラインで反応なしならば
    {
        if (fieldDeg() > 90 && fieldDeg() < 270)
        {
            defenderVec = getVec(fieldDeg(), 75);
        }
        else
        {
            if (fieldDeg() > 180) // 右のコートの中心があるなら
            {
                defenderVec = getVec(225, 75);
            }
            else
            {
                defenderVec = getVec(135, 75);
            }
        }
    }

    bool isDangerBallPosi = abs(diffDeg(defenderVec.deg(), irDeg())) < 80 && irDis() < 100.0; // 移動方向と同じ方向にボールがあると危険

    if (!lineEscapeMovement && isDangerBallPosi) // ラインが反応していないなら
    {
        playAttacker(Attacker::Mode::GYRO); // 攻撃の動き（よける）
    }
    else
    {
        motorsVectorMove(&defenderVec); // ディフェンダーの動き
    }
}

// ディフェンス
static PD pdLineTrace(0.30, 0.0); // ライントレース用のPD調節値

static Vector Defence(int power, bool useTaikaku, bool irDefence)
{

    int resetDeg = (yellowGoalDetected() && useTaikaku) ? (yellowGoalDeg() + 180) % 360 : 0; // ゴールの対角方向の角度

    int ballDeg = irDeg();                               // backToLineMiddleがtrueならラインの中心に戻るようにする
    int taikakuIrDeg = (ballDeg - resetDeg + 360) % 360; // ゴールの対角方向の角度を基準とした時のIR角度

    /* --詳しい算出方法は物xを見るべし-- */
    double maxLen = (double)power; // ディフェンス時の最大長さ
    const int irFrontDeg = 25;     // 対角線を基準としたIRの角度の前の範囲（この角度を区切りにロボットは停止する制御に移る）
    const int yellowFrontDeg = 20; // 黄色ゴールの角度の前の範囲（この角度を区切りにロボットは停止する制御に移る）

    pdLineTrace.process(lineRingDis(), 0.0); // ライントレース用PD計算

    // ライトレース用ベクトル
    double lineLen = constrain(fabs((double)pdLineTrace.output()), 0.0, maxLen);
    Vector lineTraceVec(lineRingDeg(), lineLen);

    // ライトレースの残りのベクトルの長さ
    double behindMaxLen = sqrt(maxLen * maxLen - lineTraceVec.length() * lineTraceVec.length());

    // 精度向上のため粗目のラインの角度を計算（矛盾）
    int lineRingRoundDeg = areaIndexFromDeg(8, lineRingDeg()) * 360 / 8;

    // IRボール用のベクトル
    // 対角方向を基準としたIRボールの方向の接線方向の算出
    int irSessenDeg = nearSeesenDeg(lineRingRoundDeg, taikakuIrDeg); // どっちの接線がIRボールの角度に近いか
    // 対角方向を基準としたIRボールの角度によってベクトルの長さを変える
    double irDegVal = (taikakuIrDeg < 180) ? (double)taikakuIrDeg : 360.0 - (double)taikakuIrDeg;
    double irLen = (taikakuIrDeg < irFrontDeg || taikakuIrDeg > 360 - irFrontDeg) ? fabs(irDegVal * behindMaxLen / (double)irFrontDeg) : behindMaxLen;
    irLen = constrain(irLen, 0.0, behindMaxLen);
    // ベクトル生成
    Vector irSessenVec(irSessenDeg, irLen);

    // ライン中心に戻る用のベクトル
    int yellowSessenDeg = nearSeesenDeg(lineRingRoundDeg, yellowGoalDeg()); // ラインの施栓方向の角度の内yellowDeg()に近いほうを返す
    // 黄色ゴールの角度によってベクトルの長さを変えながら中心に戻る // 後ろを0度としてvalueを計算
    double yellowDegVal = yellowGoalDeg() - 180;
    double yellowLen = abs(yellowDegVal) < yellowFrontDeg ? fabs(yellowDegVal * behindMaxLen / (double)yellowFrontDeg) : behindMaxLen;
    yellowLen = constrain(yellowLen, 0.0, behindMaxLen);
    // ベクトル生成
    Vector yellowSessenVec(yellowSessenDeg, yellowLen);

    // 移動ベクトルの合成(irDefenceがfalseの時はラインの中心に戻るベクトルの成分を足す)
    Vector moveVec = !irDefence ? lineTraceVec + yellowSessenVec : lineTraceVec + irSessenVec;
    if (moveVec.length() > maxLen)
    {
        moveVec = moveVec * (maxLen / moveVec.length());
    }

    return moveVec;

    /*
    Serial.print("lineTraceVec(" + String(lineTraceVec.deg()) + "," + String(lineTraceVec.length()) + ")");
    Serial.print(" + ");
    Serial.print("irSessenVec(" + String(irSessenVec.deg()) + "," + String(irSessenVec.length()) + ")");
    Serial.print(" = ");
    Serial.println("moveVec(" + String(moveVec.deg()) + "," + String(moveVec.length()) + ")");
    */
}

// ゴールと距離保ちながら戻る
static PD pdKeepDisFromGoal(4.8, 0); // ゴールと一定の距離を保ちながら定位置へ移動用のPD調整値

static Vector TeiitiModoru(int power, int keepDis)
{
    double maxLen = (double)power; // ディフェンス時の最大長さ

    pdKeepDisFromGoal.process(yellowGoalDis(), keepDis); // pd計算

    int keepDisDeg = yellowGoalDis() < keepDis ? yellowGoalDeg() + 180 : yellowGoalDeg() + 0; // ゴールからの距離によって移動方向をかえる
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

    return moveVec;
}

static Vector LineEscape(int power)
{
    int moveDeg = lineRingDeg() + 180;

    if (abs(diffDeg(lineRingDeg(), fieldDeg())) > 90)
    {
        moveDeg = lineRingDeg();
    }

    Vector moveVec(moveDeg, power);

    return moveVec;
}