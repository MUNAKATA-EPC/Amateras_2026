#pragma once

#include <Arduino.h>

namespace catchSensor
{
    enum Mode
    {
        NONE = LOW,
        CATCH = HIGH,
    };
}

namespace motorToggle
{
    enum Mode
    {
        OFF = LOW,
        ON = HIGH,
    };
}