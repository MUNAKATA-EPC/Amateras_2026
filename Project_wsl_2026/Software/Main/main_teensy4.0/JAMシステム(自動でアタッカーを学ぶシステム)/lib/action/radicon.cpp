#include "radicon.hpp"

void radicon(int max_power);

void playRadicon(Radicon::Mode mode)
{
    fullColorLed1.rgbLightUp(0, 0, 0); // リセット

    if (mode == Radicon::Mode::ZENRYOKU)
    {
        radicon(80);
    }
    else if (mode == Radicon::Mode::DARADARA)
    {
        radicon(50);
    }
}

void radicon(int power)
{
    // PD計算
    int target_deg = ps3RightStickDetected() ? -ps3RightStickDeg() : 0;
    motorsPdProcess(&pd_gyro, bnoDeg(), target_deg);

    // キッカー
    kicker1.kick(
        ps3ButtonIsPushing(Ps3Button::L1) ||
        ps3ButtonIsPushing(Ps3Button::L2) ||
        ps3ButtonIsPushing(Ps3Button::R1) ||
        ps3ButtonIsPushing(Ps3Button::R2));

    // ps3からの読み取り　移動方向の計算
    int move_deg = normalizeDeg(ps3LeftStickDeg() + bnoDeg());
    int move_power = power * ps3LeftStickDis() / 128.0f;
    if (ps3ButtonIsPushing(Ps3Button::CIRCLE)) // ○ボタンで全力
    {
        move_power = 95.0f;
    }
    if (!ps3LeftStickDetected())
    {
        move_deg = 0xFF;
        move_power = 0xFF;
    }

    // 制御
    if (move_deg != 0xFF)
    {
        motorsMove(move_deg, move_power);
    }
    else
    {
        motorsPdMove();
    }
}