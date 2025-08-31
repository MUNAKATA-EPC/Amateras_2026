#include "action.hpp"
#include "driver.hpp"
#include "function.hpp"
#include "sensor.hpp"

void setup()
{
    IR ir(Serial1, 115200, 0xAA);
    LINE line(Serial2, 115200, 0xAA);
}

void loop()
{
    ir.update();
    line.update();
}