/*センサーからの読み取り*/
#pragma once

#include "BallSensor/BallSensor.hpp"
#include "LineSensor/LineSensor.hpp"
#include "Camera/Openmv.hpp"
#include "Gyro/BNO055.hpp"
#include "DigitalSensor/DigitalSensor.hpp"
#include "Button/Button.hpp"

class Sensors
{
public:
    BallSensor ir;
    LineSensor line;
    Openmv cam;
    BNO055 bno;
    DigitalSensor catchSensor;

    Sensors();
    void begin();
    void update();
};

// グローバル参照（宣言のみ）
extern Sensors sensors;
extern DigitalSensor &catchSensor;
extern BallSensor &ir;
extern LineSensor &line;
extern Openmv &cam;
extern BNO055 &bno;
