#include "Actions.hpp"
#include "Drivers.hpp"
#include "Functions.hpp"
#include "Sensors.hpp"
#include <Servo.h>

/*
#define ESC_PIN 9
#define MIN 1000
#define MAX 2000
Servo esc;
*/

enum PrintMode
{
    ALL,
    IR,
    LINE,
    GYRO,
    CAM
};
void playPrint(PrintMode mode); // シリアルプリントする関数

Timer timer; // ui更新用のタイマー

void setup()
{
    // PCとのシリアル通信の開始
    Serial.begin(9600);
    // センサー類の開始
    SensorsBegin();
    // ドライバー類の開始
    DriversBegin();
    // サーボモータの開始
    /*
    esc.attach(ESC_PIN);
    esc.writeMicroseconds(MAX);
    delay(500);
    esc.writeMicroseconds(MIN);
    delay(500);
    */
}

void loop()
{
    // センサー類の更新
    SensorsUpdate();

    // uiを実行
    if ((!timer.everCalled() || timer.msTime() > 10) && !ui.running()) // まだ呼ばれていない場合もタイマーをリセットさせる
    {
        timer.reset();
        ui.process(true, enterButton.isReleased(), rightButton.isReleased(), leftButton.isReleased()); // 10msに一回更新
    }
    else
    {
        ui.process(false, enterButton.isReleased(), rightButton.isReleased(), leftButton.isReleased());
    }

    // ドリブラーを動かす
    // esc.writeMicroseconds((MIN + MAX) / 2);

    // 動作を実行
    if (!ui.running())
    {
        Serial.println("now sellecting...");
        kicker.kick(false); // キッカーを動かさない
        motors.stop();      // 動作選択中はモータを止める
    }
    else
    {
        playPrint(PrintMode::ALL);
        switch (ui.actionNumber())
        {
        case Action::ATTACKER:
            playAttacker((Attacker::Mode)ui.modeNumber(), 95);
            break;
        case Action::DEFENDER:
            break;
        case Action::TEST:
            break;
        case Action::RADICON:
            break;
        }
    }

    kicker.kick(resetButton.isReleased());
}

void playPrint(PrintMode mode)
{
    switch (mode)
    {
    case PrintMode::ALL:
    case PrintMode::IR:
        Serial.print("ir.deg:");
        Serial.print(ir.deg());
        if (mode == PrintMode::IR)
        {
            Serial.print(" ir.val:");
            Serial.print(ir.val());
            break;
        }
    case PrintMode::LINE:
        Serial.print(" line.deg:");
        Serial.print(line.deg());
        if (mode == PrintMode::LINE)
        {
            Serial.print(" line.dis:");
            Serial.print(line.dis());
            break;
        }
    case PrintMode::GYRO:
        Serial.print(" bno.deg:");
        Serial.print(bno.deg());
        if (mode == PrintMode::GYRO)
        {
            break;
        }
    case PrintMode::CAM:
        Serial.print(" yellow.deg:");
        Serial.print(cam.yellowGoal.deg());
        if (mode == PrintMode::CAM)
        {
            Serial.print(" yellow.dis:");
            Serial.print(cam.yellowGoal.dis());
        }
        Serial.print(" blue.deg:");
        Serial.print(cam.blueGoal.deg());
        if (mode == PrintMode::CAM)
        {
            Serial.print(" blue.dis:");
            Serial.print(cam.blueGoal.dis());
            break;
        }
    }
    Serial.println();
}