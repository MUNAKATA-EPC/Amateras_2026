#include "defender.hpp"

static AnglePD pdGyro(0.8, 0.1); // ジャイロ用のPD調節値
static AnglePD pdCam(0.6, 0.1);  // カメラ用のPD調節値

static PD pdLineTrace(0.78, 0.0); // ライントレース用のPD調節値

void playDefender()
{
    motorsPdProcess(&pdGyro, bnoDeg(), 0);

    pdLineTrace.process(lineRingDetected() ? lineRingDis() : 0.0, 0.0); // ライントレース用PD計算

    int resetDeg = yellowGoalDetected() ? (yellowGoalDeg() + 180) % 360 : 0; // ゴールの対角方向の角度
    int taikakuIrDeg = (irDeg() - resetDeg + 360) % 360;                     // ゴールの対角方向の角度を基準とした時のIR角度

    if (lineRingDetected())
    {
        /* --詳しい算出方法は物xを見るべし-- */
        const double defenceMaxLen = 90.0; // ディフェンス時の最大長さ

        // ライトレース用ベクトル
        double lineLen = constrain(fabs((double)pdLineTrace.output()), 0.0, defenceMaxLen);
        Vector lineTraceVec(lineRingDeg(), lineLen);

        // IRボール用ベクトル（ラインの角度の接線方向）（ライトレースの残りのベクトルで生成）
        double irMaxLen = sqrt(defenceMaxLen * defenceMaxLen - lineTraceVec.length() * lineTraceVec.length());

        int lineRingRoundDeg = areaIndexFromDeg(8, lineRingDeg()) * 360 / 8;
        int sessen1 = (lineRingRoundDeg + 90) % 360, sessen2 = (lineRingRoundDeg + 270) % 360;

        bool isSessen1Near = abs(diffDeg(sessen1, taikakuIrDeg)) < abs(diffDeg(sessen2, taikakuIrDeg));

        int irSessen = isSessen1Near ? sessen1 : sessen2; // どっちの接線がIRボールの角度に近いか
        Vector irSessenVec(irSessen, irMaxLen);

        // 移動ベクトルの合成
        bool isTaikakuIrFront = taikakuIrDeg < 10 || taikakuIrDeg > 350;
        Vector moveVec = irDetected() ? !isTaikakuIrFront ? lineTraceVec + irSessenVec : lineTraceVec : lineTraceVec;
        if (moveVec.length() > defenceMaxLen)
        {
            moveVec = moveVec * (defenceMaxLen / moveVec.length());
        }

        motorsVectorMove(&moveVec);

        Serial.print(lineTraceVec.deg());
        Serial.print(" , ");
        Serial.print(lineTraceVec.length());
        Serial.print(" = ");
        Serial.print(moveVec.deg());
        Serial.print(" , ");
        Serial.println(moveVec.length());
    }
    else
    {
        // 円形ラインで反応なし
        Serial.print("none -> ");
        Serial.println(yellowGoalDeg());

        if (yellowGoalDetected())
        {
            motorsMove(yellowGoalDeg(), 50);
        }
        else
        {
            motorsPdMove();
        }
    }
}
