#include "Actions.hpp"
#include "Drivers.hpp"
#include "Functions.hpp"
#include "Sensors.hpp"

// 使用するセンサー類
DigitalSensor catchSensor; // kickerのキャッチセンサー
DigitalSensor camToggle;   // カメラのトグルスイッチ
DigitalSensor motorToggle; // モータのトグルスイッチ

Button resetButton; // ジャイロのリセットボタン
Button enterButton; // Uiの決定ボタン
Button rightButton; // Uiの右ボタン
Button leftButton;  // Uiの左ボタン

BallSensor ir;
LineSensor line;
Openmv cam;

void setup()
{
    Serial.begin(9600);

    catchSensor.init(6);
    camToggle.init(5);
    motorToggle.init(5);

    resetButton.init(9, INPUT_PULLDOWN);
    enterButton.init(11, INPUT_PULLDOWN);
    rightButton.init(12, INPUT_PULLDOWN);
    leftButton.init(10, INPUT_PULLDOWN);

    ir.init(&Serial1, 115200, 0xAA);
    line.init(&Serial2, 115200, 0xAA);
    cam.init(&Serial3, 115200, 0xAA);
}

void loop()
{
    Serial.println(catchSensor.read() == DigitalSensor::CATCH);

    delay(100);
}