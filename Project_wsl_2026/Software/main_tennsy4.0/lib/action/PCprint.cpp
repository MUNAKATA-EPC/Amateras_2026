#include "PCprint.hpp"

/*パソコンにシリアルプリント用のプログラムを実行する*/

void play_PCprint(int PCprint_mode)
{
    switch (PCprint_mode)
    {
    case TEST_ALL_CHECK_WITH_SERIAL:
        // IRについて
        Serial.print(" ir_deg: ");
        Serial.print(get_IR_deg());
        Serial.print(" ir_dis: ");
        Serial.print(get_IR_distance());
        // LINEについて
        Serial.print(" line_side: r");
        Serial.print(get_LINE_side_right());
        Serial.print("l");
        Serial.print(get_LINE_side_left());
        Serial.print("b");
        Serial.print(get_LINE_side_back());
        Serial.print(" line_deg: ");
        Serial.print(get_LINE_deg());
        // ジャイロについて
        Serial.print(" bno_deg: ");
        Serial.print(get_BNO055_deg());
        // カメラについて
        Serial.print(" field_deg: ");
        Serial.print(get_field_deg());
        Serial.print(" yellow_deg: ");
        Serial.print(get_yellow_goal_deg());
        Serial.print(" blue_deg: ");
        Serial.print(get_blue_goal_deg());

        Serial.println();
        break;

    case TEST_IR_CHECK_WITH_SERIAL:
        // IRについて
        Serial.print(" ir_deg: ");
        Serial.print(get_IR_deg());
        Serial.print(" ir_dist: ");
        Serial.print(get_IR_distance());
        // IRcomputeについて
        Serial.print(" -> ir_mawarikomi_deg: ");
        Serial.print(get_IR_mawarikomi_deg());
        Serial.print(" ir_follow_deg: ");
        Serial.print(get_IR_follow_deg(0));

        Serial.println();
        break;

    case TEST_LINE_CHECK_WITH_SERIAL:
        // LINEについて
        Serial.print(" line_data ");
        for (int i = 0; i < 16; i++)
            Serial.print(get_LINE_data(i));
        Serial.print(" line_side: r");
        Serial.print(get_LINE_side_right());
        Serial.print("l");
        Serial.print(get_LINE_side_left());
        Serial.print("b");
        Serial.print(get_LINE_side_back());
        Serial.print(" line_deg: ");
        Serial.print(get_LINE_deg());

        Serial.println();
        break;

    case TEST_GYRO_CHECK_WITH_SERIAL:
        // ジャイロについて
        Serial.print(" bno_deg: ");
        Serial.print(get_BNO055_deg());
        Serial.print(" pd_power: ");
        PD_use_gyro(); // ジャイロで計算させる
        Serial.print(get_PD_power());

        Serial.println();
        break;

    case TEST_CAM_CHECK_WITH_SERIAL:
        // カメラについて
        Serial.print(" field_deg: ");
        Serial.print(get_field_deg());
        Serial.print(" yellow_deg: ");
        Serial.print(get_yellow_goal_deg());
        Serial.print(" yellow_dis: ");
        Serial.print(get_yellow_goal_distance());
        Serial.print(" blue_deg: ");
        Serial.print(get_blue_goal_deg());
        Serial.print(" blue_dis: ");
        Serial.print(get_blue_goal_distance());

        Serial.println();
        break;

    default:
        Serial.println("error");
        break;
    }
}