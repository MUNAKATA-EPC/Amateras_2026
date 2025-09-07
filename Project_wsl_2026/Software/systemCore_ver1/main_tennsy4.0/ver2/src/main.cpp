#include "Actions.hpp"
#include "Drivers.hpp"
#include "Functions.hpp"
#include "Sensors.hpp"

Timer timer; // ui更新用のタイマー

void setup()
{
    // PCとのシリアル通信の開始
    Serial.begin(9600);
    // センサー類の開始
    SensorsBegin();
    // ドライバー類の開始
    DriversBegin();
}

void loop()
{
    // センサー類の更新
    SensorsUpdate();

    // uiを実行
    if (!timer.everCalled() || timer.msTime() > 100) // まだ呼ばれていない場合もタイマーをリセットさせる
    {
        timer.reset();
        ui.process(); // 100msに一回更新
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
        switch (ui.actionNumber())
        {
        case Action::ATTACKER:
            playAttacker(ui.modeNumber(), 95);
            break;
        case Action::DEFENDER:
            break;
        case Action::TEST:
            break;
        case Action::RADICON:
            break;
        }
    }
}