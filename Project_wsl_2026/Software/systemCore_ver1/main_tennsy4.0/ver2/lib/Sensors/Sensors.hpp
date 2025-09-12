/*センサーからの読み取り*/
#pragma once

#include "BallSensor/BallSensor.hpp"
#include "LineSensor/LineSensor.hpp"
#include "Camera/Openmv.hpp"
#include "Gyro/BNO055.hpp"
#include "DigitalSensor/DigitalSensor.hpp"
#include "Button/Button.hpp"

extern Button resetButton;
extern Button enterButton;
extern Button rightButton;
extern Button leftButton;
extern BallSensor ir;
extern LineSensor line;
extern Openmv cam;
extern BNO055 bno;
extern DigitalSensor catchSensor;

// すべてのセンサーを開始させる関数
void SensorsBegin();
// すべてのセンサーを更新させる関数
void SensorsUpdate();
