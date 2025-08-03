#include "IRcompute.hpp"

/*得たIRボールの状況から回り込む角度などを算出する*/

int get_IR_mawarikomi_deg()
{
    if (get_IR_deg() < 45) // 左にボールがある
    {
    }
    else if (get_IR_deg() < 90)
    {
    }
    else if (get_IR_deg() < 135)
    {
    }
    else if (get_IR_deg() < 180)
    {
    }
    else if (get_IR_deg() < 225) // 右にボールがある
    {
    }
    else if (get_IR_deg() < 270)
    {
    }
    else if (get_IR_deg() < 315)
    {
    }
    else
    {
    }
}

int get_IR_follow_deg(int follow_target_offset)
{
}