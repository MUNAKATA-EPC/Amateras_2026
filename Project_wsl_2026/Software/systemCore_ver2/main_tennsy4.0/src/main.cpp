#include "Actions.hpp"
#include "Drivers.hpp"
#include "Functions.hpp"
#include "Sensors.hpp"

// 使用するセンサー類
BallSensor ir;             // IRセンサー
LineSensor line;           // ラインセンサー
DigitalSensor catchSensor; // kickerのキャッチセンサー
DigitalSensor motorToggle; // モータのトグルスイッチ
Button enterButton;        // Uiの決定ボタン
Button rightButton;        // Uiの右ボタン
Button leftButton;         // Uiの左ボタン
BNO055 bno;                // ジャイロセンサー
Openmv cam;                // カメラ

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

    bno.init(&Wire1, 0x28, 9, INPUT_PULLDOWN);

    cam.init(&Serial3, 115200, 0xAA);
}

void loop()
{
    Serial.println(catchSensor.read() == DigitalSensor::CATCH);

    delay(100);
}