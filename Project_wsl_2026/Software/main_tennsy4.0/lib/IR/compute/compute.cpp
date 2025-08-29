#include "compute.hpp"
#include <cmath>

/*得たIRボールの状況から回り込む角度などを算出する*/

int get_IR_mawarikomi_deg()
{
    int IR_mawarikomi_deg; // 回り込むための角度格納用
    int ir_deg = get_IR_deg();

    if (ir_deg < 45) // 左にボールがある
    {
        IR_mawarikomi_deg = ir_deg + 65;
    }
    else if (ir_deg < 90)
    {
        IR_mawarikomi_deg = ir_deg + 65;
    }
    else if (ir_deg < 135)
    {
        IR_mawarikomi_deg = ir_deg + 65;
    }
    else if (ir_deg < 180)
    {
        IR_mawarikomi_deg = ir_deg + 65;
    }
    else if (ir_deg < 225) // 右にボールがある
    {
        IR_mawarikomi_deg = ir_deg - 65;
    }
    else if (ir_deg < 270)
    {
        IR_mawarikomi_deg = ir_deg - 65;
    }
    else if (ir_deg < 315)
    {
        IR_mawarikomi_deg = ir_deg - 65;
    }
    else
    {
        IR_mawarikomi_deg = ir_deg - 65;
    }

    IR_mawarikomi_deg = (IR_mawarikomi_deg + 360) % 360; // 一応0~360に直す

    return IR_mawarikomi_deg; // 回り込む角度を返す
}

// 角度の算出方法について : https://www.desmos.com/calculator/c0nzjtscrs?lang=ja
// ロボットは原点にして障害物（円）との接線をIRedge[rad] ± asin(ballR / IRdist)で求める方式
int get_IR_sessen_deg(double R, double dist_adjust_value)
{
    int dist = get_IR_distance() + dist_adjust_value;
    if (dist > R) // 接線が存在する場合  ->  && !LeftBack_and_RightBack
    {
        int edge = (get_IR_deg() + 360 - 270) % 360; // x軸正の方向を0度とする

        double rad_edge = radians(edge);      // ラジアンの角度を計算
        double rad_dif_edge = asin(R / dist); // ± asin(ballR ÷ IRdist)

        int sessen_angle1 = int(degrees(rad_edge + rad_dif_edge)); // 接点1を求める
        sessen_angle1 = (sessen_angle1 + 360) % 360;               // 一応丸める
        sessen_angle1 = (sessen_angle1 + 360 - 90) % 360;          // 前を0度とする

        int sessen_angle2 = int(degrees(rad_edge - rad_dif_edge)); // 接点2を求める
        sessen_angle2 = (sessen_angle2 + 360) % 360;               // 一応丸める
        sessen_angle2 = (sessen_angle2 + 360 - 90) % 360;          // 前を0度とする

        if (90 < edge && edge < 270) // 左にボールがある時
        {
            return sessen_angle1;
        }
        else // 右にある時
        {
            return sessen_angle2;
        }
    }
    else
    {
        return get_IR_mawarikomi_deg(); // 接線が存在しないときは回り込みの角度を返す
    }
}

int get_IR_follow_deg(int follow_target_offset)
{
    if (follow_target_offset == 0)
        return get_IR_deg(); // 近寄る目標がないときはそのままの角度を返す

    int ir_deg = get_IR_deg();
    int ir_distance = get_IR_distance();

    double delta_x = cos(radians(ir_deg)) * ir_distance;                        // xの変化量
    double delta_y = sin(radians(ir_deg)) * ir_distance - follow_target_offset; // yの変化量

    int IR_follow_deg = (int)(degrees(atan2(delta_y, delta_x))); // 角度算出
    IR_follow_deg = (IR_follow_deg + 360) % 360;                 // 0~360に直す

    return IR_follow_deg; // 近寄る角度を返す
}

int get_IR_hirei_deg(double a)
{
    int ir_deg = get_IR_deg();
    int IR_value = (ir_deg < 180) ? ir_deg : ir_deg - 360; // 現在の角度を-180~180に変換
    int IR_hirei_deg = (int)(IR_value * a);                // 比例して角度を調整
    IR_hirei_deg = (IR_hirei_deg + 360) % 360;             // 一応0~360に直す

    return IR_hirei_deg; // 比例角度を返す
}

int get_IR_hirei_distance(double a, int dist_adjust_value)
{
    int value = get_IR_value() + dist_adjust_value; // 近ければ大きく、遠ければ小さくする

    if (value < 0 || value < 60) // ボールが明らかに遠かったら0を返す
        return 0;

    return int(a * value);
}