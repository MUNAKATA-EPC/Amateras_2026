#include "defender.hpp"

static PD pdGyro(0.8, 0.1); // ジャイロ用のPD調節値
static PD pdCam(0.6, 0.1);  // カメラ用のPD調節値

static PD pdLineX(1.0, 0.45); // ライントレース用のPD調節値

void playDefender()
{
    motorsPdProcess(&pdGyro, bnoDeg(), 0);

    pdLineX.process(lineRingX(), 0); // ライントレース用PD計算

    if (lineRingDetected())
    {
        const double lineTraceMaxLen = 95.0; // ライントレースの最大長さ

        /*
        // 円形ラインでPD制御する
        Serial.print(lineRingDeg());
        Serial.print(" , ");
        Serial.println(lineRingX());
        */

        // ライトレースのx成分は、角度=(リングのx角度)と長さ=(リングのx成分)で決定
        int xDeg = lineRingDeg() > 135 && lineRingDeg() < 255 ? 180 : 0;
        double xLen = constrain(fabs((double)pdLineX.output()), 0.0, lineTraceMaxLen);
        Vector xVec(xDeg, xLen);

        // ライトレースのy成分は、角度=(IRのy角度)と長さ=(100-リングの長さ)で決定
        int yDeg = irDeg() > 180 ? 270 : 90;
        // 合力が0になってほしいので三平方の定理で算出
        double yLen = sqrt(lineTraceMaxLen * lineTraceMaxLen - xVec.length() * xVec.length());
        Vector yVec = Vector(yDeg, yLen);

        // 移動ベクトルの合成
        Area16 irPosi = area16(irDeg()); // IRボールが前にあるかどうか
        if (irPosi == Area16::FRONT)
        {
            yVec = yVec * 0.0; // IRボールがないのならy成分は削除
        }
        else if (irPosi == Area16::FRONT_FRONT_LEFT || irPosi == Area16::FRONT_FRONT_RIGHT)
        {
            yVec = yVec * 0.5; // IRボールが微妙に前にあるならy成分を弱める
        }
        Vector moveVec = xVec + yVec;

        // motorsMove(xVec.deg(), xVec.length());
        motorsVectorMove(&moveVec);

        Serial.print(xVec.deg());
        Serial.print(" , ");
        Serial.print(xVec.length());
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