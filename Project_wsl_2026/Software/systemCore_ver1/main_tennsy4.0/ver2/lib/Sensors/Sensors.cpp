#include "Sensors.hpp"

// ボールセンサー
BallSensor ir(&Serial1, 115200, 0xAA);
// ラインセンサー
LineSensor line(&Serial5, 115200, 0xAA);
// カメラ
Openmv cam(&Serial3, 115200, 0xAA);
// ジャイロ
BNO055 bno(&Wire, 0x28);
Button resetButton(9, INPUT_PULLDOWN); //<-ここでしか使えない
// キャッチセンサー
DigitalSensor catchSensor(6);

void SensorsBegin()
{
    ir.begin();
    line.begin();
    cam.begin();
    bno.begin(&resetButton);
}

void SensorsUpdate()
{
    ir.update();
    line.update();
    cam.update();
    bno.update();
}
