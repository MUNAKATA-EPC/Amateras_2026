#include "Actions.hpp"
#include "Drivers.hpp"
#include "Functions.hpp"
#include "Sensors.hpp"

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
    ui.process();
    Serial.print(" action: ");
    Serial.print(ui.actionDecided());
    Serial.print(" ");
    Serial.print(ui.actionNumber());
    Serial.print(" mode: ");
    Serial.print(ui.modeDecided());
    Serial.print(" ");
    Serial.print(ui.modeNumber());
    Serial.print(" config: ");
    Serial.print(ui.configNumber());
    Serial.print(" ");
    Serial.println(ui.configActive(ui.configNumber()));
}