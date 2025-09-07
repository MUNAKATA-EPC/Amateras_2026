#include "Sensors.hpp"

// 定義
Sensors::Sensors()
    : ir(&Serial1, 115200, 0xAA),
      line(&Serial5, 115200, 0xAA),
      cam(&Serial3, 115200, 0xAA),
      bno(&Wire, 0x28),
      catchSensor(6)
{
}

// 開始
void Sensors::begin()
{
    ir.begin();
    line.begin();
    cam.begin();
    bno.begin(&Button(9, INPUT_PULLDOWN));
}

// 更新
void Sensors::update()
{
    ir.update();
    line.update();
    cam.update();
    bno.update();
}

// ショートカット
Sensors sensors;
DigitalSensor &catchSensor = sensors.catchSensor;
BallSensor &ir = sensors.ir;
LineSensor &line = sensors.line;
Openmv &cam = sensors.cam;
BNO055 &bno = sensors.bno;
