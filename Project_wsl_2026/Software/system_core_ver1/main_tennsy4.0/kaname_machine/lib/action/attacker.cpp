#include "attacker.hpp"

static PD pdGyro(0.55, 1.6); // ジャイロ用のPD調節値

void playAttacker(Attacker::Mode mode)
{
    motorsPdProcess(&pdGyro, bnoDeg(), 0);

    if (irY() > -70.0f && irY() < 70.0f) // 真正面
    {
        motorsMove(0, 50);
    }
    else if (irY() > -180.0f && irY() < 180.0f) // 右前や左前
    {
        const float d = 270.0f;
        Vector move_vector(d, 0.0f, irX(), irY());

        move_vector = mapVec(move_vector, move_vector.length(), 50.0f);

        motorsVectorMove(&move_vector);
    }
    else
    {
        float diff_deg;

        if (irDis() < 500.0f) // 近い
            diff_deg = irVal() * 0.10f;
        else if (irDis() < 750.0f) // 中くらい
            diff_deg = irVal() * 0.06f;
        else // 遠い
            diff_deg = 0.0f;

        int move_deg = irDeg() + int(irDeg() < 0 ? -diff_deg : diff_deg);

        motorsMove(move_deg, 50);
    }
}

/*
#define LINE_MEMORY
// #define LINE_FIELD

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

    if (mode == Attacker::Mode::YELLOWGOAL || mode == Attacker::Mode::BLUEGOAL)
    {
        if (lineRingDetected()) // コート方式
        {
            Serial.print(String(fieldDeg()) + " " + String(lineRingDeg()) + " " + String(diffDeg(fieldDeg(), lineRingDeg())));

            if (abs(diffDeg(fieldDeg(), lineRingDeg())) < 115) // ラインが半分以上出てしまったら
            {
                line_deg = lineRingDeg() + 180; // 今のライン角度と反対側に行く

                line_escape = true; // ラインから離れる
            }
            else
            {
                power = (int)round(map(lineRingDis(), 0.0f, 100.0f, (float)max_power * 0.2f, (float)max_power * 0.6f)); // 出力を下げる

                // line_deg = lineRingDeg(); // 今のライン角度
                // line_escape = true;       // ラインから離れる
            }
        }

        // ゴールが見えたらline_degはゴール方向
        if (yellowGoalDetected() && yellowGoalDis() < 55)
        {
            line_deg = yellowGoalDeg();
            line_escape = true; // ゴールから離れる
        }

        if (blueGoalDetected() && blueGoalDis() < 55)
        {
            line_deg = blueGoalDeg();
            line_escape = true; // ゴールから離れる
        }
    }
    else
    {
        int lineSensor_memory_count = 0; // 初期化

        if (lineRingDetected()) // 記憶方式
        {
            float x = 0.0f, y = 0.0f;

            for (uint8_t i = 0; i < 16; i++)
            {
                if (lineSensorDetected(i) == true || lineSensor_memory[i] == true)
                {
                    x += cosf(radians(22.5f * i));
                    y += sinf(radians(22.5f * i));
                    lineSensor_memory[i] = true;

                    lineSensor_memory_count++;
                }
            }

            line_deg = (int)round(degrees(atan2f(y, x)));

            line_escape = true;
        }
        else
        {
            for (uint8_t i = 0; i < 16; i++)
                lineSensor_memory[i] = false;
        }
    }

    const int ir_near_dis = 555; // IRセンサーが近いと判断する距離

    // 制御
    if (lineSideDetected())
    {
        if (lineSideRightDetected() && lineSideLeftDetected())
        {
            motorsMove(fieldDeg(), power);
        }
        else
        {
            motorsMove(lineSideDeg() + 180, power);
        }
    }
    else if (line_escape)
    {
        motorsMove(line_deg + 180, power);
    }
    else if (irDetected())
    {
        const int front_deg = 10;
        const int near_front_deg = 25;

        if (abs(irDeg()) <= front_deg)
        {
            motorsMove(0, power);
        }
        else if (abs(irDeg()) <= near_front_deg)
        {
            int deg = irDeg() * (near_front_deg + 40) / (near_front_deg);
            motorsMove(deg, power * 0.81);
        }
        else
        {
            float diffMawarikomiDeg = 0.0f;
            if (irDis() <= ir_near_dis)
            {
                diffMawarikomiDeg = (float)irVal() * 0.09f;

                if (abs(irDeg()) >= 90)
                    diffMawarikomiDeg += 10.0f;
                else
                    diffMawarikomiDeg += 10.0f;
            }
            else
                diffMawarikomiDeg = 0.0f;

            motorsMove(irDeg() > 0 ? irDeg() + (int)diffMawarikomiDeg : irDeg() - (int)diffMawarikomiDeg, power);
        }
    }
    else
    {
        motorsPdMove();
    }
}
*/