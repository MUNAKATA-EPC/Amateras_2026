#include "Action.hpp"
#include "Driver.hpp"
#include "Function.hpp"
#include "Sensor.hpp"

// sensors
DigitalSensor catchSensor;
DigitalSensor camToggle;
DigitalSensor motorToggle;
IR ir;
Line line;

void setup()
{
    Serial.begin(9600);

    catchSensor.init(6);
    ir.init(&Serial1, 115200, 0xAA);
    line.init(&Serial2, 115200, 0xAA);
}

void loop()
{
    Serial.println(catchSensor.read() == DigitalSensor::CATCH);

    delay(100);
}