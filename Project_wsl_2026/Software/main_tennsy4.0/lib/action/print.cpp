#include "print.hpp"

/*パソコンにシリアルプリント用のプログラムを実行する*/

void play_pc_print(int pc_print_mode)
{
    switch (pc_print_mode)
    {
    case ALL_CHECK_WITH_PC:
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

    case IR_CHECK_WITH_PC:
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

    case LINE_CHECK_WITH_PC:
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

    case GYRO_CHECK_WITH_PC:
        // ジャイロについて
        Serial.print(" bno_deg: ");
        Serial.print(get_BNO055_deg());
        Serial.print(" pd_power: ");
        PD_use_gyro(); // ジャイロで計算させる
        Serial.print(get_PD_power());

        Serial.println();

        break;

    case CAM_CHECK_WITH_PC:
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

/*LCDにプリントさせる用のプログラムを実行する*/

void play_lcd_print(int lcd_print_mode, int ahead_x, int ahead_y)
{
    String line_print_data = ""; // LINE_CHECK_WITH_LCDで使う

    switch (lcd_print_mode)
    {
    case IR_LINE_GYRO_CHECK_WITH_LCD:
        // IRについて
        SSD1306_write(1, 0 + ahead_x, 0 + ahead_y, "ir_deg: " + String(get_IR_deg()), false);
        SSD1306_write(1, 0 + ahead_x, 10 + ahead_y, "ir_dis:" + String(get_IR_distance()), false);
        // LINEについて
        SSD1306_write(1, 0 + ahead_x, 20 + ahead_y, "line_deg: " + String(get_LINE_deg()), false);
        SSD1306_write(1, 0 + ahead_x, 30 + ahead_y, "line_side: " + String(get_LINE_side_right()) + String(get_LINE_side_left()) + String(get_LINE_side_back()), false);
        // ジャイロについて
        SSD1306_write(1, 0 + ahead_x, 40 + ahead_y, "bno_deg: " + String(get_BNO055_deg()), false);
        SSD1306_write(1, 0 + ahead_x, 50 + ahead_y, "pd_power: " + String(get_PD_power()), false);

        break;

    case IR_CHECK_WITH_LCD:
        // IRserialについて
        SSD1306_write(1, 0 + ahead_x, 0 + ahead_y, "ir_deg: " + String(get_IR_deg()), false);
        SSD1306_write(1, 0 + ahead_x, 10 + ahead_y, "dis_deg:" + String(get_IR_distance()), false);
        // IRcomputeについて
        SSD1306_write(1, 0 + ahead_x, 20 + ahead_y, "mawari_deg: " + String(get_IR_mawarikomi_deg()), false);
        SSD1306_write(1, 0 + ahead_x, 30 + ahead_y, "follow_deg:" + String(get_IR_follow_deg(0)), false);

        break;

    case LINE_CHECK_WITH_LCD:
        // LINEselial・computeについて
        SSD1306_write(1, 0 + ahead_x, 0 + ahead_y, "line_deg: " + String(get_LINE_deg()), false);
        for (int i = 0; i < 19; i++)
            line_print_data = line_print_data + String(get_LINE_data(i));
        SSD1306_write(1, 0 + ahead_x, 10 + ahead_y, "line_data: " + line_print_data, false);

        break;

    case GYRO_CHECK_WITH_LCD:
        // ジャイロについて
        SSD1306_write(1, 0 + ahead_x, 0 + ahead_y, "bno_deg: " + String(get_BNO055_deg()), false);
        PD_use_gyro();
        SSD1306_write(1, 0 + ahead_x, 10 + ahead_y, "pd_power: " + String(get_PD_power()), false);

        break;

    case CAM_CHECK_WITH_LCD:
        // カメラについて
        SSD1306_write(1, 0 + ahead_x, 0 + ahead_y, "yellow_deg: " + String(get_yellow_goal_deg()), false);
        SSD1306_write(1, 0 + ahead_x, 10 + ahead_y, "yellow_dis: " + String(get_yellow_goal_distance()), false);
        SSD1306_write(1, 0 + ahead_x, 20 + ahead_y, "blue_deg: " + String(get_blue_goal_deg()), false);
        SSD1306_write(1, 0 + ahead_x, 30 + ahead_y, "blue_dis: " + String(get_blue_goal_distance()), false);

        break;

    default:
        SSD1306_write(1, 0 + ahead_x, 0 + ahead_y, "error", false);
        break;
    }
}