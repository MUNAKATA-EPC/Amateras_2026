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
    sensors.begin();
    // uiの初期設定・開始
    ui.begin(&enterButton, &rightButton, &leftButton);
}

void loop()
{
    // センサー類の更新
    sensors.update();

    // uiを実行
    if (!timer.everCalled() || timer.msTime() > 100) // まだ呼ばれていない場合もタイマーをリセットさせる
    {
        timer.reset();
        ui.process(); // 100msに一回更新
    }
}