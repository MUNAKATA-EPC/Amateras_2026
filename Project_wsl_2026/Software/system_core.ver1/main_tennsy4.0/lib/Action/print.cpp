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
        Serial.print(get_LINE_memory_deg());
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
        Serial.print(get_LINE_memory_deg());

        Serial.println();

        break;

    case GYRO_CHECK_WITH_PC:
        // ジャイロについて
        Serial.print(" bno_deg: ");
        Serial.print(get_BNO055_deg());

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

    case PS3_CHECK_WITH_PC:
        // Ps3について
        Serial.print(" stick_left_deg: ");
        Serial.print(get_Ps3_stick_left_deg());
        Serial.print(" stick_left_distance: ");
        Serial.print(get_Ps3_stick_left_distance());
        Serial.print(" stick_right_deg: ");
        Serial.print(get_Ps3_stick_right_deg());
        Serial.print(" stick_right_distance: ");
        Serial.print(get_Ps3_stick_right_distance());
        Serial.print(" stick_left_move: ");
        Serial.print(is_Ps3_stick_left_move());
        Serial.print(" stick_lx: ");
        Serial.print(get_Ps3_stick_lx());
        Serial.print(" stick_ly: ");
        Serial.print(get_Ps3_stick_ly());
        Serial.print(" stick_right_move: ");
        Serial.print(is_Ps3_stick_right_move());
        Serial.print(" stick_rx: ");
        Serial.print(get_Ps3_stick_rx());
        Serial.print(" stick_ry: ");
        Serial.print(get_Ps3_stick_ry());

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
    char buf[64];             // 十分なバッファサイズを確保
    char line_print_data[64]; // LINEデータ用

    switch (lcd_print_mode)
    {
    case IR_LINE_GYRO_CHECK_WITH_LCD:
        // IRについて
        sprintf(buf, "ir_deg: %d", get_IR_deg());
        SSD1306_write(1, 0 + ahead_x, 0 + ahead_y, buf, false);
        sprintf(buf, "ir_deg: %d", get_IR_distance());
        SSD1306_write(1, 0 + ahead_x, 10 + ahead_y, buf, false);

        // LINEについて
        sprintf(buf, "line_deg: %d", get_LINE_memory_deg());
        SSD1306_write(1, 0 + ahead_x, 20 + ahead_y, buf, false);
        line_print_data[0] = '\0';
        for (int i = 16; i <= 18; i++)
        {
            char tmp[4];
            sprintf(tmp, "%d", get_LINE_data(i));
            strcat(line_print_data, tmp);
        }
        sprintf(buf, "line_side: %s", line_print_data);
        SSD1306_write(1, 0 + ahead_x, 30 + ahead_y, buf, false);

        // ジャイロについて
        sprintf(buf, "bno_deg: %d", get_BNO055_deg());
        SSD1306_write(1, 0 + ahead_x, 40 + ahead_y, buf, false);
        sprintf(buf, "pd_power: %d", get_PD_power());
        SSD1306_write(1, 0 + ahead_x, 50 + ahead_y, buf, false);
        break;

    case IR_CHECK_WITH_LCD:
        sprintf(buf, "ir_deg: %d", get_IR_deg());
        SSD1306_write(1, 0 + ahead_x, 0 + ahead_y, buf, false);
        sprintf(buf, "dis_deg: %d", get_IR_distance());
        SSD1306_write(1, 0 + ahead_x, 10 + ahead_y, buf, false);
        sprintf(buf, "mawari_deg: %d", get_IR_mawarikomi_deg());
        SSD1306_write(1, 0 + ahead_x, 20 + ahead_y, buf, false);
        sprintf(buf, "follow_deg: %d", get_IR_follow_deg(0));
        SSD1306_write(1, 0 + ahead_x, 30 + ahead_y, buf, false);
        break;

    case LINE_CHECK_WITH_LCD:
        sprintf(buf, "line_deg: %d", get_LINE_memory_deg());
        SSD1306_write(1, 0 + ahead_x, 0 + ahead_y, buf, false);

        line_print_data[0] = '\0';
        for (int i = 0; i < 19; i++)
        {
            char tmp[4];
            sprintf(tmp, "%d", get_LINE_data(i));
            strcat(line_print_data, tmp);
        }
        sprintf(buf, "line_data: %s", line_print_data);
        SSD1306_write(1, 0 + ahead_x, 10 + ahead_y, buf, false);
        break;

    case GYRO_CHECK_WITH_LCD:
        sprintf(buf, "bno_deg: %d", get_BNO055_deg());
        SSD1306_write(1, 0 + ahead_x, 0 + ahead_y, buf, false);
        break;

    case CAM_CHECK_WITH_LCD:
        sprintf(buf, "yellow_deg: %d", get_yellow_goal_deg());
        SSD1306_write(1, 0 + ahead_x, 0 + ahead_y, buf, false);
        sprintf(buf, "yellow_dis: %d", get_yellow_goal_distance());
        SSD1306_write(1, 0 + ahead_x, 10 + ahead_y, buf, false);
        sprintf(buf, "blue_deg: %d", get_blue_goal_deg());
        SSD1306_write(1, 0 + ahead_x, 20 + ahead_y, buf, false);
        sprintf(buf, "blue_dis: %d", get_blue_goal_distance());
        SSD1306_write(1, 0 + ahead_x, 30 + ahead_y, buf, false);
        break;

    default:
        SSD1306_write(1, 0 + ahead_x, 0 + ahead_y, "error", false);
        break;
    }
}
