#include "jam.hpp"

void playJAM(JAM::Mode mode)
{
    fullColorLed1.rgbLightUp(0, 0, 0); // リセット

    if (mode == JAM::Mode::MANABU)
    {
    }
    else if (mode == JAM::Mode::MIRU)
    {
    }
    else if (mode == JAM::Mode::KAKIKOMU)
    {
    }
    else if (mode == JAM::Mode::ATTACKER)
    {
    }
}

void printJAM(JAM::Mode mode)
{
    if (mode == JAM::Mode::MANABU)
    {
        uiPrint(0, 0, "hello");
    }
    else if (mode == JAM::Mode::MIRU)
    {
        uiPrint(0, 0, "hello");
    }
    else if (mode == JAM::Mode::KAKIKOMU)
    {
        uiPrint(0, 0, "hello");
    }
    else if (mode == JAM::Mode::ATTACKER)
    {
        uiPrint(0, 0, "hello");
    }
}