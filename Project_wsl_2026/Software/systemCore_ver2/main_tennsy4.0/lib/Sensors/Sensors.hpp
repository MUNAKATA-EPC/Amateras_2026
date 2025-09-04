/*センサーからの情報を読み取り*/
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
    Button resetButton;
    BNO055 bno;
    DigitalSensor catchSensor;
    DigitalSensor motorToggle;
    Button enterButton;
    Button rightButton;
    Button leftButton;

    Sensors()
        : // ボールセンサー
          ir(&Serial1, 115200, 0xAA),
          // ラインセンサー
          line(&Serial5, 115200, 0xAA),
          // カメラ
          cam(&Serial3, 115200, 0xAA),
          // ジャイロ
          resetButton(9, INPUT_PULLDOWN),
          bno(&Wire, 0x28),
          // キャッチセンサー
          catchSensor(6),
          // モータ用のトグルスイッチ
          motorToggle(5),
          // ディスプレイ
          enterButton(11, INPUT_PULLDOWN),
          rightButton(12, INPUT_PULLDOWN),
          leftButton(10, INPUT_PULLDOWN)
    {
    }

    void begin()
    {
        /*センサー類の開始*/
        // シリアル
        ir.begin();
        line.begin();
        cam.begin();
        // I2C
        bno.begin(&resetButton);
        // ボタン
        resetButton.begin();
        enterButton.begin();
        rightButton.begin();
        leftButton.begin();
    }

    void update()
    {
        /*センサー類の更新*/
        // シリアル
        ir.update();
        line.update();
        cam.update();
        // I2C
        bno.update();
        // ボタン
        resetButton.update();
        enterButton.update();
        rightButton.update();
        leftButton.update();
    }
};

/*定義*/
Sensors sensors;
// ショートカット参照を作成
auto &ir = sensors.ir;
auto &line = sensors.line;
auto &cam = sensors.cam;
auto &bno = sensors.bno;
auto &catchSensor = sensors.catchSensor;
auto &motorToggle = sensors.motorToggle;
auto &enterButton = sensors.enterButton;
auto &rightButton = sensors.rightButton;
auto &leftButton = sensors.leftButton;
auto &resetButton = sensors.resetButton;