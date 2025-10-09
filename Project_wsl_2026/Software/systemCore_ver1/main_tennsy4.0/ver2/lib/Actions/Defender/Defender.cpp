#include "Defender/Defender.hpp"

void playDefender(int power)
{
    Defender::Mode mode = (Defender::Mode)ui.modeNumber();

    // PD制御
    int pd_deg = 0;
    PD *pd = &pdGyro; // デフォルトはジャイロ
    pd_deg = bno.deg();
    motors.PDprocess(pd, pd_deg, 0); // PD成分計

    // 攻撃/守備する方向を格納
    if (mode == Defender::Mode::GYRO)
    {
        playAttacker(power);
        return;
    }

    // 守備/攻撃の方向を計算
    Openmv::CamData attackGoal = Openmv::CamData::YELLOWGOAL;
    Openmv::CamData defenceGoal = Openmv::CamData::BLUEGOAL;
    if (mode == Defender::Mode::BLUEGOAL)
    {
        attackGoal = Openmv::CamData::BLUEGOAL;
        defenceGoal = Openmv::CamData::YELLOWGOAL;
    }

    // ディフェンス！
    if (area4(cam.deg(defenceGoal)) == Area4::BACK && cam.dis(defenceGoal) < 100 && line.detected()) // 後ろにゴールがあり、距離も近く、ラインも反応しているとき
    {
        int topIndex = areaIndexFromDeg(16, line.deg());
        bool topFiveDetected = line.sensor(topIndex - 2) || line.sensor(topIndex - 1) || line.sensor(topIndex) || line.sensor(topIndex + 1) || line.sensor(topIndex + 2);

        if (line.dis() < 0.2) // エンジェルライン中心にラインが来ている
        {
            int sessen1 = (line.deg() + 90) % 360;
            int sessen2 = (line.deg() + 270) % 360;

            if (diffDeg(ir.deg(), sessen1) < diffDeg(ir.deg(), sessen2)) // 2接線のうちirの角度に近いほうへ行く
            {
                motors.move(sessen1, power * 0.8);
            }
            else
            {
                motors.move(sessen2, power * 0.8);
            }
        }
        else if (topFiveDetected) // ラインの先頭の5個が反応している
        {
            motors.move(line.deg(), power * 0.8);
        }
        else
        {
            Vector sum = getVec(line.deg(), 10) + getVec(ir.deg(), 10);

            motors.move(sum.deg(), power * 0.8);
        }
    }
    else
    {
        motors.PDmove();
    }
}