#include "radicon.hpp"
static PD pdGyro(0.8f, 0.1f); // ジャイロ用のPD調節値
static PD pdCam(0.6f, 0.1f);  // カメラ用のPD調節値

static int record[360] = {0xFF}; // 0~359度、距離10段階の記録用配列

void radiconRecord()
{
    // キッカー
    bool on = (catchSensor.read() == HIGH) || ps3ButtonIsPushing(ButtonDataType::L3) || ps3ButtonIsPushing(ButtonDataType::R3);
    kicker1.kick(on);

    // コントローラー
    int pd_stick_deg = ps3LeftStickDeg();
    bool pd_stick_detected = ps3LeftStickDetected();

    int move_stick_deg = ps3RightStickDeg();
    bool move_stick_detected = ps3RightStickDetected();

    // PD制御
    if (pd_stick_detected)
    {
        motorsPdProcess(&pdGyro, bnoDeg(), -pd_stick_deg); // PD成分計算(右ステック補正付き)
    }
    else
    {
        motorsPdProcess(&pdGyro, bnoDeg(), 0); // PD成分計算
    }

    // パワー
    int power = 85;

    // 制御
    if (move_stick_detected)
    {
        int moveDeg = normalizeDeg(move_stick_deg + bnoDeg()); // 移動方向
        float scale = ps3LeftStickDis() / 128.0f;              // 速度のスケール計算

        motorsMove(moveDeg, (int)power * scale);

        // 記録処理
        if (irDetected())
        {
            int ir_deg_record_index = (irDeg() + 360) % 360; // 0~359度に変換

            record[ir_deg_record_index] = moveDeg; // 記録
        }
    }
    else
    {
        motorsPdMove();
    }
}

void radiconComplement()
{
}

void radiconReplay()
{
}

void playRadicon()
{
    Radicon::Mode mode = (Radicon::Mode)uiModeNumber();

    switch (mode)
    {
    case Radicon::Mode::RECORD:
        radiconRecord();
        break;
    case Radicon::Mode::COMPLEMENT:
        radiconComplement();
        break;
    case Radicon::Mode::REPLAY:
        radiconReplay();
        break;
    default:
        motorsStop();
        break;
    }
}