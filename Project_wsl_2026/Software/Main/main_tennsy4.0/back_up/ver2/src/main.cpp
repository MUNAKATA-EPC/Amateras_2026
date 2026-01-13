#include "Actions.hpp"
#include "Drivers.hpp"
#include "Functions.hpp"
#include "Sensors.hpp"

enum PrintMode
{
    ALL,
    IR,
    LINE,
    GYRO,
    CAM,
    PS3
};
void playPrint(PrintMode mode); // シリアルプリントする関数

Timer uiTimer; // ui更新用のタイマー

void setup()
{
    // PCとのシリアル通信の開始
    Serial.begin(9600);
    // ドライバー類の開始
    DriversBegin();
    // センサー類の開始
    SensorsBegin();
}

void loop()
{
    // センサー類の更新
    SensorsUpdate();

    // ドリブラーを動かす
    if (!esc.available())
        esc.setup();
    else
        esc.move(1000);

    // uiを実行
    if (!uiTimer.everCalled() || uiTimer.msTime() > 10) // まだ呼ばれていない場合もタイマーをリセットさせる
    {
        uiTimer.reset();
        bool show = !ui.running(); // runningなら表示しない
        ui.process(show, enterButton.isReleased(), rightButton.isReleased(), leftButton.isReleased()); // 10msに一回更新
    }

    // 動作を実行
    if (!ui.running())
    {
        Serial.println("now sellecting...");
        kicker.kick(false); // キッカーを動かさない
        motors.stop();      // 動作選択中はモータを止める
    }
    else
    {
        // playPrint(PrintMode::LINE);
        switch (ui.actionNumber())
        {
        case Action::ATTACKER:
            playAttacker(95);
            break;
        case Action::DEFENDER:
            playDefender(66);
            break;
        case Action::TEST:
            break;
        case Action::RADICON:
            playRadicon(95);
            break;
        }
    }
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
        Serial.print(line.deg(LineSensor::RING));
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
        Serial.print(cam.deg(Openmv::YELLOWGOAL));
        if (mode == PrintMode::CAM)
        {
            Serial.print(" yellow.dis:");
            Serial.print(cam.dis(Openmv::YELLOWGOAL));
        }
        Serial.print(" blue.deg:");
        Serial.print(cam.deg(Openmv::BLUEGOAL));
        if (mode == PrintMode::CAM)
        {
            Serial.print(" blue.dis:");
            Serial.print(cam.dis(Openmv::BLUEGOAL));
            break;
        }
    case PrintMode::PS3:
        if (mode == PrintMode::PS3)
        {
            Serial.print("right.deg:");
            Serial.print(ps3.deg(M5stamp::StickData::RIGHTSTICK));
            Serial.print(" right.dis:");
            Serial.print(ps3.dis(M5stamp::StickData::RIGHTSTICK));
            Serial.print(" left.deg:");
            Serial.print(ps3.deg(M5stamp::StickData::LEFTSTICK));
            Serial.print(" left.dis:");
            Serial.print(ps3.dis(M5stamp::StickData::LEFTSTICK));
            Serial.print(" button:");
            for (int i = 0; i < 14; i++)
            {
                Serial.print(ps3.button((M5stamp::ButtonType)i));
            }
            break;
        }
    }
    Serial.println();
}