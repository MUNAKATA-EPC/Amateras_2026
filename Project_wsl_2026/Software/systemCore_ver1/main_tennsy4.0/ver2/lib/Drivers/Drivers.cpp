#include "Drivers.hpp"

// キッカー
Kicker kicker(7, 8);
// モーター
Motors motors(&Serial2, 115200);
// UI
UI ui(&Wire, 0x3C, 128, 64);
Button enterButton(11, INPUT_PULLDOWN); //<-ここでしか使えない
Button rightButton(12, INPUT_PULLDOWN); //<-ここでしか使えない
Button leftButton(10, INPUT_PULLDOWN);  //<-ここでしか使えない

void DriversBegin()
{
    kicker.begin(700);
    ui.begin(&enterButton, &rightButton, &leftButton);
    motors.begin(&DigitalSensor(5));
    motors.setMoveSign(1, -1, -1, 1);         // 移動のための符号をセット
    motors.setPdSign(1, 1, 1, 1);             // PD制御のための符号をセット
    motors.setDegPosition(315, 45, 225, 135); // モータの位置をセット
}