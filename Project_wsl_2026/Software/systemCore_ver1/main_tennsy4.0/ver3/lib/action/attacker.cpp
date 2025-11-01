#include "attacker.hpp"

// #define LINE_MEMORY
#define LINE_FIELD

static PD pdGyro(0.4, 1.6); // ジャイロ用のPD調節値
static PD pdCam(0.9, 1.1);  // カメラ用のPD調節値

static Timer camTimer;
static Timer lineTimer;

static Movement_average move_deg_ave;

#ifdef LINE_MEMORY
static bool lineSensor_memory[16] = {false};
#endif

void playAttacker(Attacker::Mode mode)
{
    // キャッチしたらゴール方向を一定時間向く
    if (catchSensor.read() == HIGH)
        camTimer.reset();

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
        if (yellowGoalDetected() && (camTimer.msTime() < 1200 && camTimer.everCalled()))
        {
            angle_pd = &pdCam;
            angle_pd_deg = yellowGoalDeg();
        }
        else if (yellowGoalDetected() && yellowGoalDis() < 99)
        {
            angle_pd = &pdCam;
            angle_pd_deg = yellowGoalDeg();

            camTimer.reset();
        }
        else
        {
            angle_pd = &pdGyro;
            angle_pd_deg = bnoDeg();
        }
    }

    motorsPdProcess(angle_pd, angle_pd_deg, 0);

    // キッカー
    kicker1.kick(catchSensor.read() == HIGH);

    // ライン計算と出力の計算
    const int max_power = 50; // パワーの最大値
    int power = max_power;    // デフォルトは最大値

    bool line_escape = false; // ラインから離れる動きをするかどうか
    int line_deg = 0xFF;      // 計算したライン角度

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

        if (abs(diffDeg(fieldDeg(), lineRingDeg())) < 90) // ラインが半分以上出てしまったら
        {
            line_deg = lineRingDeg() + 180; // 今のライン角度と反対側に行く

            line_escape = true; // ラインから離れる
        }
        else
        {
            // power = (int)round(map(lineRingDis(), 0.0, 100.0, (double)max_power * 0.4, (double)max_power) * 0.6); // 出力を下げる

            line_deg = lineRingDeg(); // 今のライン角度
            line_escape = true;       // ラインから離れる
        }
    }

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
#endif

    int move_deg = 0;
    int move_power = 0.0;
    bool move_deg_ave_reset = false;
    move_deg_ave.set(5); // 5個の平均をとる
    // 制御
    if (line_escape)
    {
        move_deg = line_deg + 180;
        move_power = power;

        move_deg_ave_reset = true;
    }
    else if (irDetected())
    {
        if (abs(irDeg()) < 9)
        {
            move_deg = 0;
            move_power = power;
        }
        else if (abs(irDeg()) < 40 && irDis() < 60)
        {
            move_deg = mapDeg(irDeg(), 20, 80, MapMode::HIREI);
            move_power = power * 0.75;
        }
        else
        {
            double diffMawarikomiDeg = (irDis() > 200) ? 0 : (irDis() > 50) ? irVal() / 3.6
                                                                            : 60;
            move_deg = (irDeg() > 0) ? irDeg() + (int)diffMawarikomiDeg : irDeg() - (int)diffMawarikomiDeg;
            move_power = power;
        }
    }
    else
    {
        move_deg = 0;
        move_power = 0;

        move_deg_ave_reset = true;
    }

    if (move_deg_ave_reset)
    {
        move_deg_ave.reset();
    }
    else
    {
        move_deg = move_deg_ave.add(move_deg);
    }

    motorsMove(move_deg, move_power);
}