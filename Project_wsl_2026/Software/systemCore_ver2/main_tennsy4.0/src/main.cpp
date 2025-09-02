#include "Actions.hpp"
#include "Drivers.hpp"
#include "Functions.hpp"
#include "Sensors.hpp"

// 使用するセンサー類
BallSensor ir;             // IRセンサー
LineSensor line;           // ラインセンサー
Openmv cam;                // カメラ
Button enterButton;        // ディスプレイの決定ボタン
Button rightButton;        // ディスプレイの右ボタン
Button leftButton;         // ディスプレイの左ボタン
Button resetButton;        // ジャイロセンサーのリセットボタン
BNO055 bno;                // ジャイロセンサー
DigitalSensor catchSensor; // キッカーのキャッチセンサー
DigitalSensor motorToggle; // モータのトグルスイッチ

void setup()
{
    // PCとのシリアル通信の定義
    Serial.begin(9600);
    // IRセンサーの定義
    ir.init(&Serial1, 115200, 0xAA);
    // ラインセンサーの定義
    line.init(&Serial2, 115200, 0xAA);
    // カメラの定義
    cam.init(&Serial3, 115200, 0xAA);
    // ディスプレイの定義
    enterButton.init(11, INPUT_PULLDOWN);
    rightButton.init(12, INPUT_PULLDOWN);
    leftButton.init(10, INPUT_PULLDOWN);
    // ジャイロの定義
    resetButton.init(9, INPUT_PULLDOWN);
    bno.init(&Wire1, 0x28, &resetButton);
    // その他のセンサー
    catchSensor.init(6);
    motorToggle.init(5);
}

void loop()
{
    // IRセンサーの更新
    ir.update();
    // ラインセンサーの更新
    line.update();
    // カメラの更新
    cam.update();
    //  ディスプレイの更新
    enterButton.update();
    rightButton.update();
    leftButton.update();
    // ジャイロの更新
    resetButton.update();
    bno.update();

    delay(100);
}