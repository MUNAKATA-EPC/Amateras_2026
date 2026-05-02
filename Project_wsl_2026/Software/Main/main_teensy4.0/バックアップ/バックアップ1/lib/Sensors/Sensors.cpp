#include "Sensors.hpp"

// ボタン
Button resetButton(9, INPUT_PULLDOWN);
Button enterButton(11, INPUT_PULLDOWN);
Button rightButton(12, INPUT_PULLDOWN);
Button leftButton(10, INPUT_PULLDOWN);
// ボールセンサー
BallSensor ir(&Serial1, 115200, 0xAA);
// ラインセンサー
LineSensor line(&Serial5, 115200, 0xAA);
// カメラ
Openmv cam(&Serial3, 115200, 0xAA);
// ジャイロ
BNO055 bno(&Wire, 0x28);
// キャッチセンサー
DigitalSensor catchSensor(6);
// PS3コントローラー
M5stamp ps3(&Serial2, 115200, 0xAA);

void SensorsBegin()
{
    resetButton.begin();
    enterButton.begin();
    rightButton.begin();
    leftButton.begin();

    ir.begin();
    line.begin();
    cam.begin();
    bno.begin();
    ps3.begin(10, 10);
}

void SensorsUpdate()
{
    resetButton.update();
    enterButton.update();
    rightButton.update();
    leftButton.update();

    ir.update();
    line.update();
    cam.update();
    bno.update(resetButton.isReleased());
    ps3.update();
}
