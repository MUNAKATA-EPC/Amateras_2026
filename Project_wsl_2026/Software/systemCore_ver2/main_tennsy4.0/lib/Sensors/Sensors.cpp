#include "Sensors.hpp"

// 定義
Sensors::Sensors()
    : ir(&Serial1, 115200, 0xAA),
      line(&Serial5, 115200, 0xAA),
      cam(&Serial3, 115200, 0xAA),
      resetButton(9, INPUT_PULLDOWN),
      bno(&Wire, 0x28),
      catchSensor(6),
      motorToggle(5),
      enterButton(11, INPUT_PULLDOWN),
      rightButton(12, INPUT_PULLDOWN),
      leftButton(10, INPUT_PULLDOWN)
{
}

// 開始
void Sensors::begin()
{
    ir.begin();
    line.begin();
    cam.begin();
    bno.begin(&resetButton);
}

// 更新
void Sensors::update()
{
    ir.update();
    line.update();
    cam.update();
    bno.update();
}

// ショートカットキー
Sensors sensors;
Button &enterButton = sensors.enterButton;
Button &rightButton = sensors.rightButton;
Button &leftButton = sensors.leftButton;
Button &resetButton = sensors.resetButton;
DigitalSensor &catchSensor = sensors.catchSensor;
DigitalSensor &motorToggle = sensors.motorToggle;
BallSensor &ir = sensors.ir;
LineSensor &line = sensors.line;
Openmv &cam = sensors.cam;
BNO055 &bno = sensors.bno;
