#include "Actions.hpp"
#include "Drivers.hpp"
#include "Functions.hpp"
#include "Sensors.hpp"

// 使用するセンサー類
BallSensor ir;   // IRセンサー
LineSensor line; // ラインセンサー
Openmv cam;      // カメラ

Button enterButton; // Uiの決定ボタン
Button rightButton; // Uiの右ボタン
Button leftButton;  // Uiの左ボタン

Button resetButton; // ジャイロセンサーのリセットボタン
BNO055 bno;         // ジャイロセンサー

DigitalSensor catchSensor; // キッカーのキャッチセンサー
DigitalSensor motorToggle; // モータのトグルスイッチ

void setup()
{
    Serial.begin(9600);

    ir.init(&Serial1, 115200, 0xAA);
    line.init(&Serial2, 115200, 0xAA);

    catchSensor.init(6);
    motorToggle.init(5);

    enterButton.init(11, INPUT_PULLDOWN);
    rightButton.init(12, INPUT_PULLDOWN);
    leftButton.init(10, INPUT_PULLDOWN);

    resetButton.init(9, INPUT_PULLDOWN);
    bno.init(&Wire1, 0x28, &resetButton);

    cam.init(&Serial3, 115200, 0xAA);
}

void loop()
{
    // IRセンサーの更新
    ir.update();
    // ラインセンサーの更新
    line.update();
    // カメラの更新
    cam.update();
    //  Uiの更新
    enterButton.update();
    rightButton.update();
    leftButton.update();
    // ジャイロの更新
    resetButton.update();
    bno.update();

    delay(100);
}