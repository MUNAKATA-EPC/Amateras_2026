#include "attacker.hpp"

// #define LINE_MEMORY
#define LINE_FIELD

static PD pdGyro(0.55, 1.6); // ジャイロ用のPD調節値
static PD pdCam(0.5, 0.2);   // カメラ用のPD調節値

static Timer lineTimer;

static int line_deg = 0xFF; // 計算したライン角度
#ifdef LINE_MEMORY
static bool lineSensor_memory[16] = {false};
#endif

void playAttacker(Attacker::Mode mode)
{
    bool goal_detected = (mode == Attacker::Mode::YELLOWGOAL) ? yellowGoalDetected() : blueGoalDetected();
    int goal_deg = (mode == Attacker::Mode::YELLOWGOAL) ? yellowGoalDeg() : blueGoalDeg();

    // PD制御
    PD *angle_pd = nullptr;
    int angle_pd_deg = 0;

    if (mode == Attacker::Mode::GYRO)
    {
        angle_pd = &pdGyro;
        angle_pd_deg = bnoDeg();
    }
    else
    {
        if (goal_detected)
        {
            angle_pd = &pdCam;

            angle_pd_deg = goal_deg;
        }
        else
        {
            angle_pd = &pdGyro;
            angle_pd_deg = bnoDeg();
        }
    }

    motorsPdProcess(angle_pd, angle_pd_deg, 0);

    // キッカー
    bool is_catching = (catchSensor.read() == HIGH);

    if (mode == Attacker::Mode::GYRO)
    {
        kicker1.kick(is_catching);
    }
    else
    {
        kicker1.kick(is_catching && goal_detected && abs(goal_deg) < 50);
    }

    // ライン計算と出力の計算
    const int max_power = 85; // パワーの最大値

    int power = max_power;    // デフォルトは最大値
    bool line_escape = false; // ラインから離れる動きをするかどうか

#ifdef LINE_MEMORY                   // 記憶方式
    int lineSensor_memory_count = 0; // 初期化

    if (lineRingDetected())
    {
        double x = 0.0, y = 0.0;

        for (uint8_t i = 0; i < 16; i++)
        {
            if (lineSensorDetected(i) == true || lineSensor_memory[i] == true)
            {
                x += cos(radians(22.5 * i));
                y += sin(radians(22.5 * i));
                lineSensor_memory[i] = true;

                lineSensor_memory_count++;
            }
        }

        line_deg = (int)round(degrees(atan2(y, x)));

        line_escape = true;
    }
    else
    {
        for (uint8_t i = 0; i < 16; i++)
            lineSensor_memory[i] = false;
    }
#endif

#ifdef LINE_FIELD // コート方式
    if (lineRingDetected())
    {
        Serial.print(String(fieldDeg()) + " " + String(lineRingDeg()) + " " + String(diffDeg(fieldDeg(), lineRingDeg())));

        if (abs(diffDeg(fieldDeg(), lineRingDeg())) < 110) // ラインが半分以上出てしまったら
        {
            line_deg = lineRingDeg() + 180; // 今のライン角度と反対側に行く

            line_escape = true; // ラインから離れる
        }
        else
        {
            // power = (int)round(map(lineRingDis(), 0.0, 100.0, (double)max_power * 0.2, (double)max_power * 0.6)); // 出力を下げる

            line_deg = lineRingDeg(); // 今のライン角度
            line_escape = true;       // ラインから離れる
        }
    }
#endif

    // ゴールが見えたらline_degはゴール方向
    if (yellowGoalDetected() && yellowGoalDis() < 70)
    {
        line_deg = yellowGoalDeg();
        line_escape = true; // ゴールから離れる
    }

    if (blueGoalDetected() && blueGoalDis() < 70)
    {
        line_deg = blueGoalDeg();
        line_escape = true; // ゴールから離れる
    }

    // 制御
    if (line_escape)
    {
        motorsMove(line_deg + 180, power);
    }
    else if (irDetected())
    {
        if (abs(irDeg()) <= 13)
        {
            motorsMove(0, power);
        } /*
        else if (abs(irDeg()) <= 18)
         {
 #define HOLD_DIS 628.0
 #define MAX_POWER 60.0

             motorsPdProcess(&pdGyro, bnoDeg(), 0);

             Vector hold_vec(HOLD_DIS, 0, irX(), irY());

             hold_vec = hold_vec * MAX_POWER / hold_vec.length();

             motorsVectorMove(&hold_vec);
         }*/
        else if (abs(irDeg()) <= 16 && irDis() < 750)
        {
            motorsMove(mapDeg(irDeg(), 16, 50, MapMode::HIREI), 50);
        }
        else
        {
            double diffMawarikomiDeg = 0.0;
            if (irDis() > 750)
                diffMawarikomiDeg = 0;
            else
                diffMawarikomiDeg = irVal() * 0.07;

            if (abs(irDeg()) > 76)
                diffMawarikomiDeg += 30.0;

            motorsMove(irDeg() > 0 ? irDeg() + (int)diffMawarikomiDeg : irDeg() - (int)diffMawarikomiDeg, power);
        }
    }
    else
    {
        motorsPdMove();
    }
}

/*
#define HOLD_DIS 30.0
#define MAX_POWER 60.0

void playAttacker(Attacker::Mode mode)
{
    motorsPdProcess(&pdGyro, bnoDeg(), 0);

    Vector hold_vec(HOLD_DIS, 0, irX(), irY());

    hold_vec = hold_vec;

    Vector forward_vec(irDeg(), 0);

    motorsVectorMove(&hold_vec);
}
*/