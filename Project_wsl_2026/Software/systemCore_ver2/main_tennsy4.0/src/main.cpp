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
}

void loop()
{
    // センサー類の更新
    sensors.update();

    Serial.print("bno:");
    Serial.print(bno.deg());
    Serial.print(" ir:");
    Serial.print(ir.deg());
    Serial.print(" line:");
    Serial.println(line.deg());

    delay(100);
}