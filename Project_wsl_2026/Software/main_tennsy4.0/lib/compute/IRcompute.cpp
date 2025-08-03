#include "IRcompute.hpp"

/*得たIRボールの状況から回り込む角度などを算出する*/

int get_IR_mawarikomi_deg()
{
    int IR_mawarikomi_deg; // 回り込むための角度格納用

    if (get_IR_deg() < 45) // 左にボールがある
    {
        IR_mawarikomi_deg = get_IR_deg() + 90;
    }
    else if (get_IR_deg() < 90)
    {
        IR_mawarikomi_deg = get_IR_deg() + 60;
    }
    else if (get_IR_deg() < 135)
    {
        IR_mawarikomi_deg = get_IR_deg() + 60;
    }
    else if (get_IR_deg() < 180)
    {
        IR_mawarikomi_deg = get_IR_deg() + 90;
    }
    else if (get_IR_deg() < 225) // 右にボールがある
    {
        IR_mawarikomi_deg = get_IR_deg() - 90;
    }
    else if (get_IR_deg() < 270)
    {
        IR_mawarikomi_deg = get_IR_deg() - 60;
    }
    else if (get_IR_deg() < 315)
    {
        IR_mawarikomi_deg = get_IR_deg() - 60;
    }
    else
    {
        IR_mawarikomi_deg = get_IR_deg() - 90;
    }

    IR_mawarikomi_deg = (IR_mawarikomi_deg + 360) % 360; // 一応0~360に直す

    return IR_mawarikomi_deg; // 回り込む角度を返す
}

int get_IR_follow_deg(int follow_target_offset)
{
    int IR_follow_deg; // 近寄る角度格納用

    double delta_x = cos(radians(get_IR_deg())) * get_IR_distance();                        // xの変化量
    double delta_y = sin(radians(get_IR_deg())) * get_IR_distance() - follow_target_offset; // yの変化量

    IR_follow_deg = (int)atan2(delta_y, delta_x); // 角度算出
    IR_follow_deg = (IR_follow_deg + 360) % 360;  // 0~360に直す

    return IR_follow_deg; // 近寄る角度を返す
}