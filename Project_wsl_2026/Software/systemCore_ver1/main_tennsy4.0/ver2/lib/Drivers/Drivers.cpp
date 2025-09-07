#include "Drivers.hpp"

// 定義
Drivers::Drivers()
    : kicker(7, 8),
      ui(&Wire, 0x3C, 128, 64),
      motors(&Serial1, 115200)
{
}

// 開始
void Drivers::begin()
{
    kicker.begin(700);
    ui.begin(&Button(11, INPUT_PULLDOWN), &Button(12, INPUT_PULLDOWN), &Button(10, INPUT_PULLDOWN));
    motors.begin(&DigitalSensor(5));
    motors.setMoveSign(1, -1, -1, 1);         // 移動のための符号をセット
    motors.setPdSign(1, 1, 1, 1);             // PD制御のための符号をセット
    motors.setDegPosition(315, 45, 225, 135); // モータの位置をセット
}

// ショートカット
Drivers drivers;
Kicker &kicker = drivers.kicker;
UI &ui = drivers.ui;
Motors &motors = drivers.motors;