#include "defender.hpp"

static AnglePD pdGyro(0.8, 0.1); // ジャイロ用のPD調節値
static AnglePD pdCam(0.6, 0.1);  // カメラ用のPD調節値

static PD pdLineTrace(1.0, 0.45); // ライントレース用のPD調節値
static PD pdIrY(0.4, 0.45);       // IRボール捕獲用横方向のPD調節値

void playDefender()
{
    motorsPdProcess(&pdGyro, bnoDeg(), 0);

    pdLineTrace.process(lineRingDis(), 0.0); // ライントレース用PD計算

    Vector irVec(irDeg(), irDis()); // IRボール捕獲用横方向のPD計算
    pdIrY.process(irVec.y(), 0.0);

    if (lineRingDetected())
    {
        /* --詳しい算出方法は物xを見るべし-- */

        const double defenceMaxLen = 95.0; // ディフェンス時の最大長さ

        // ライトレースは、角度=(リングの角度)と長さ=(リングの距離)で決定
        double traceLen = constrain(fabs((double)pdLineTrace.output()), 0.0, defenceMaxLen);
        Vector lineTraceVec(lineRingDeg(), traceLen);

        // IRボールのy成分は、角度はIRのy角度(90度か270度)と長さはライントレースのベクトルと足して長さが100以下になるように決定
        // 合力が0になってほしいので三平方の定理で算出
        int yDeg = (irDeg() < 180) ? 90 : 270;
        double yLen = fabs((double)pdIrY.output());

        // y軸水平方向での円の接点からx軸までの距離
        double yMaxLen = sqrt(defenceMaxLen * defenceMaxLen - lineTraceVec.x() * lineTraceVec.x());

        if (irDeg() < 180)
        {
            yLen = constrain(yLen, 0.0, yMaxLen + lineTraceVec.y());
        }
        else
        {
            yLen = constrain(yLen, 0.0, yMaxLen - lineTraceVec.y());
        }
        Vector yVec = Vector(yDeg, yLen);

        // 移動ベクトルの合成
        Vector moveVec = lineTraceVec + yVec;

        motorsVectorMove(&moveVec);

        Serial.print(lineTraceVec.deg());
        Serial.print(" , ");
        Serial.print(lineTraceVec.length());
        Serial.print(" + ");
        Serial.print(yVec.deg());
        Serial.print(" , ");
        Serial.print(yVec.length());
        Serial.print(" = ");
        Serial.print(moveVec.deg());
        Serial.print(" , ");
        Serial.println(moveVec.length());
    }
    else
    {
        // 円形ラインで反応なし
        Serial.println("none");

        motorsPdMove();
    }
}