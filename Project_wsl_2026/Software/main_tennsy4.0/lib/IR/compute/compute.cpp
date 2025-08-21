#include "compute.hpp"
#include <cmath>

/*得たIRボールの状況から回り込む角度などを算出する*/

int16_t get_IR_mawarikomi_deg()
{
    int16_t IR_mawarikomi_deg; // 回り込むための角度格納用
    int16_t ir_deg = get_IR_deg();

    if (ir_deg < 45) // 左にボールがある
    {
        IR_mawarikomi_deg = ir_deg + 70;
    }
    else if (ir_deg < 90)
    {
        IR_mawarikomi_deg = ir_deg + 56;
    }
    else if (ir_deg < 135)
    {
        IR_mawarikomi_deg = ir_deg + 55;
    }
    else if (ir_deg < 180)
    {
        IR_mawarikomi_deg = ir_deg + 70;
    }
    else if (ir_deg < 225) // 右にボールがある
    {
        IR_mawarikomi_deg = ir_deg - 70;
    }
    else if (ir_deg < 270)
    {
        IR_mawarikomi_deg = ir_deg - 55;
    }
    else if (ir_deg < 315)
    {
        IR_mawarikomi_deg = ir_deg - 55;
    }
    else
    {
        IR_mawarikomi_deg = ir_deg - 70;
    }

    IR_mawarikomi_deg = (IR_mawarikomi_deg + 360) % 360; // 一応0~360に直す

    return IR_mawarikomi_deg; // 回り込む角度を返す
}

int16_t get_IR_follow_deg(int16_t follow_target_offset)
{
    if (follow_target_offset == 0)
        return get_IR_deg(); // 近寄る目標がないときはそのままの角度を返す

    int16_t ir_deg = get_IR_deg();
    int16_t ir_distance = get_IR_distance();

    double delta_x = cos(radians(ir_deg)) * ir_distance;                        // xの変化量
    double delta_y = sin(radians(ir_deg)) * ir_distance - follow_target_offset; // yの変化量

    int16_t IR_follow_deg = (int16_t)(degrees(atan2(delta_y, delta_x))); // 角度算出
    IR_follow_deg = (IR_follow_deg + 360) % 360;                         // 0~360に直す

    return IR_follow_deg; // 近寄る角度を返す
}

int16_t get_IR_hirei_deg(double a)
{
    int16_t ir_deg = get_IR_deg();
    int16_t IR_value = (ir_deg < 180) ? ir_deg : ir_deg - 360; // 現在の角度を-180~180に変換
    int16_t IR_hirei_deg = (int16_t)(IR_value * a);            // 比例して角度を調整
    IR_hirei_deg = (IR_hirei_deg + 360) % 360;                 // 一応0~360に直す

    return IR_hirei_deg; // 比例角度を返す
}
