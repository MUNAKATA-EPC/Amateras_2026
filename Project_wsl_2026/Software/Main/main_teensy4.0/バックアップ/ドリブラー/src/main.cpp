#include <Arduino.h>
#include "dribbler.hpp"

void setup()
{
    dribbler1.init(23);
    while (dribbler1.isInitMoving())
    {
        dribbler1.initMoving();
    }
}

void loop()
{
    dribbler1.move(1300);
}