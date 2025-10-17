#include "attacker.hpp"

static PD *pdGyro = new PD(0.6, 0.1); // ジャイロ用のPD調節値
static PD *pdCam = new PD(0.6, 0.1);  // カメラ用のPD調節値

void playAttacker()
{
    // PD制御
    motorsPdProcess(pdGyro, bnoDeg(), 0);

    // キッカー
    kicker1.kick(catchSensor.read() == HIGH);

    // 制御
    if (irDetected())
    {
        Area16 area = area16(irDeg());

        switch (area)
        {
        case Area16::FRONT:
        case Area16::FRONT_FRONT_LEFT:
            motorsMove(0, 85);
            break;

        case Area16::FRONT_LEFT:
        case Area16::LEFT_FRONT_LEFT:
        case Area16::LEFT:
            motorsMove(250, 85);
            break;

        case Area16::LEFT_BACK_LEFT:
        case Area16::BACK_LEFT:
            motorsMove(210, 85);
            break;

        case Area16::BACK_BACK_LEFT:
            motorsMove(180, 85);
            break;

        case Area16::BACK:
            motorsMove(120, 85);
            break;

        case Area16::BACK_BACK_RIGHT:
        case Area16::BACK_RIGHT:
        case Area16::RIGHT_BACK_RIGHT:
            motorsMove(180, 85);
            break;

        case Area16::RIGHT:
        case Area16::RIGHT_FRONT_RIGHT:
        case Area16::FRONT_RIGHT:
            motorsMove(110, 85);
            break;

        case Area16::FRONT_FRONT_RIGHT:
            motorsMove(15, 85);
            break;

        default:
            // もし他のエリアがあれば安全策として停止や処理を書く
            motorsMove(0, 0);
            break;
        }
    }
    else
    {
        motorsPdMove();
    }
}