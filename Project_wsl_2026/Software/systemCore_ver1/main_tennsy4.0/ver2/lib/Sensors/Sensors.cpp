#include "Drivers.hpp"
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
    if (resetButton.begin() && enterButton.begin() && rightButton.begin() && leftButton.begin())
    {
        ui.debug("buttons found");
    }
    else
    {
        ui.debug("buttons not found");
        delay(2500);
    }

    if (ir.begin() == true)
    {
        ui.debug("ir found");
    }
    else
    {
        ui.debug("ir not found");
        delay(2500);
    }

    if (line.begin() == true)
    {
        ui.debug("line found");
    }
    else
    {
        ui.debug("line not found");
        delay(2500);
    }

    if (cam.begin() == true)
    {
        ui.debug("cam found");
    }
    else
    {
        ui.debug("cam not found");
        delay(2500);
    }

    if (bno.begin() == true)
    {
        ui.debug("bno found");
    }
    else
    {
        ui.debug("bno not found");
        delay(2500);
    }

    if (ps3.begin(10, 10) == true)
    {
        ui.debug("ps3 found");
    }
    else
    {
        ui.debug("ps3 not found");
        delay(2500);
    }
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
