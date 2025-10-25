#include "defender.hpp"

// defenceで使う行動パターン
static void Defence(int power, bool useTaikaku, bool backToLineMiddle);
static void TeiitiModoru(int power, int keepDis);
static void LineEscape(int power);

static AnglePD pdGyro(0.8, 0.1); // ジャイロ用のPD調節値
static AnglePD pdCam(0.6, 0.1);  // カメラ用のPD調節値

void playDefender()
{
    motorsPdProcess(&pdGyro, bnoDeg(), 0);

    if (yellowGoalDetected()) // 黄色ゴール見えてて
    {
        const int backDeg = 45;

        if (yellowGoalDeg() > 180 - backDeg && yellowGoalDeg() < 180 + backDeg) // 後ろにゴールあって
        {
            if (lineRingDetected())
            {
                const int TaikakuBackDeg = 15; // 対角線で守備する角度

                bool backToMiddleLine = (irDis() > 160) ? true : false; // 一定距離以上だったらラインの中心に戻る

                if (yellowGoalDeg() > 180 - TaikakuBackDeg && yellowGoalDeg() < 180 + TaikakuBackDeg) // 後ろにゴールあって
                {
                    Defence(90, false, backToMiddleLine);
                }
                else
                {
                    Defence(90, true, backToMiddleLine);
                }
            }
            else
            {
                if (yellowGoalDis() < 82)
                {
                    motorsMove(yellowGoalDeg() + 180, 60);
                }
                else
                {
                    motorsMove(yellowGoalDeg(), 60);
                }
            }
        }
        else
        {
            if (lineRingDetected())
            {
                motorsMove(fieldDeg(), 90);
            }
            else
            {
                TeiitiModoru(60, 90);
            }
        }
    }
    else if (lineRingDetected()) // ラインが反応していてるならラインから逃れる動き
    {
        // LineEscape(90);
        motorsMove(fieldDeg(), 60);

        Serial.println("f");
    }
    else
    {
        // 円形ラインで反応なし
        if (fieldDeg() > 90 && fieldDeg() < 270)
        {
            motorsMove(fieldDeg(), 75);

            Serial.println("g");
        }
        else
        {
            if (fieldDeg() > 180) // 右のコートの中心があるなら
            {
                motorsMove(225, 75);

                Serial.println("h");
            }
            else
            {
                motorsMove(135, 75);

                Serial.println("i");
            }
        }
    }
}

// ディフェンス
static PD pdLineTrace(0.36, 0.0); // ライントレース用のPD調節値

static void Defence(int power, bool useTaikaku, bool backToLineMiddle)
{

    int resetDeg = (yellowGoalDetected() && useTaikaku) ? (yellowGoalDeg() + 180) % 360 : 0; // ゴールの対角方向の角度

    int ballDeg = irDeg();                               // backToLineMiddleがtrueならラインの中心に戻るようにする
    int taikakuIrDeg = (ballDeg - resetDeg + 360) % 360; // ゴールの対角方向の角度を基準とした時のIR角度

    /* --詳しい算出方法は物xを見るべし-- */
    double maxLen = (double)power; // ディフェンス時の最大長さ
    const int irFrontDeg = 20;     // 対角線を基準としたIRの角度の前の範囲（この角度を区切りにロボットは停止する制御に移る）
    const int yellowFrontDeg = 10; // 黄色ゴールの角度の前の範囲（この角度を区切りにロボットは停止する制御に移る）

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

    // 移動ベクトルの合成(backToLineMiddleが有効もしくはIRボールがない時はラインの中心に戻るベクトルの成分を足す)
    Vector moveVec = (backToLineMiddle || !irDetected()) ? lineTraceVec + yellowSessenVec : lineTraceVec + irSessenVec;
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
static PD pdKeepDisFromGoal(4.8, 0); // ゴールと一定の距離を保ちながら定位置へ移動用のPD調整値

static void TeiitiModoru(int power, int keepDis)
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

    motorsVectorMove(&moveVec);
}

static void LineEscape(int power)
{
    int moveDeg = lineRingDeg() + 180;

    if (abs(diffDeg(lineRingDeg(), fieldDeg())) > 90)
    {
        moveDeg = lineRingDeg();
    }

    motorsMove(moveDeg, power);
}