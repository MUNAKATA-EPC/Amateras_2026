#pragma once

namespace Action
{
    enum Type
    {
        JAM,
        RADICON,

        COUNT
    };
}

namespace JAM
{
    enum Mode
    {
        MANABU,
        KEISAN,
        KAKUNIN,
        KAKIKOMU,
        ATTACKER,

        MODE_COUNT
    };
}

namespace Radicon
{
    enum Mode
    {
        ZENRYOKU,
        DARADARA,

        MODE_COUNT
    };
}
