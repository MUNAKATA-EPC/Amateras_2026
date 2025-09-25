#include "Drivers.hpp"

// キッカー
Kicker kicker(2, 3);
// モーター
Motors motors(&Serial1, 115200);
DigitalSensor motorsToggle(5); //<-ここでしか使えない
// UI
UI ui(&Wire1, 0x3C, 128, 64);
// ドリブラー
Dribbler esc(13, 1000, 2000);

void DriversBegin()
{
    kicker.begin(700);
    ui.begin();
    esc.begin();
    motors.begin(&motorsToggle);
    motors.setMoveSign(1, -1, -1, 1);         // 移動のための符号をセット
    motors.setPdSign(1, 1, 1, 1);             // PD制御のための符号をセット
    motors.setDegPosition(315, 45, 225, 135); // モータの位置をセット
}