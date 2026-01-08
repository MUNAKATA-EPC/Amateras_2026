#pragma once

namespace Action
{
    enum Type
    {
        ATTACKER,
        DEFENDER,
        TEST,
        RADICON,

        COUNT
    };
}

namespace Attacker
{
    enum Mode
    {
        YELLOWGOAL,
        BLUEGOAL,
        GYRO,

        MODE_COUNT
    };
}

namespace Defender
{
    enum Mode
    {
        YELLOWGOAL,
        BLUEGOAL,

        MODE_COUNT
    };
}

namespace Test
{
    enum Mode
    {
        KICKER,
        GYRO,
        CAM,
        LINE_TRACE,

        MODE_COUNT
    };
}

namespace Radicon
{
    enum Mode
    {
        RECORD,
        REPLAY,
        ASOBI,

        MODE_COUNT
    };
}
