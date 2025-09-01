#include "action.hpp"
#include "driver.hpp"
#include "function.hpp"
#include "sensor.hpp"

// sensors
bitSensor catchsensor;
IR irball;
LINE line;

void setup()
{
    Serial.begin(9600);

    catchsensor.init(6);
    irball.init(&Serial1, 115200, 0xAA);
    line.init(&Serial2, 115200, 0xAA);
}

void loop()
{
    Serial.println(catchsensor.get());

    delay(100);
}